/*2020.05.26  complete  bakaragame-server 
        2015152042 Hong-eui-sung

 Baccarat game code using multi-thread and mutex

 Features to add in the future - side batting( Tie / Pair)
                               - main thread contains user information (file input and output whenever possible)
                               - all input exception handling
                               - code optimization
                               - Up to 7 people, up to 10 rooms in one room
                                                                                */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100   //buffer size 
#define MAX_CLNT 8  //Maximum number of people that can be connected

void *handle_clnt(void *arg);         
void *create_thread(void *arg);
void *Start_game(void *arg);

void clarify_number(const char* str, char* buf);   //money -> dot every three digit
void error_handling(char *message);
/*Game progress function */
void shuffle_card();   
int split_card();
void cal_result();
void menu();


int clnt_cnt=0;            //current number of connecting client
int ready_user_cnt=0;      //number of users bet 
pthread_mutex_t mutx;

int gameStart=0;            //Variable to control game start
int whoWIN=0;               //Win = 1 /lose = 0/ draw = 2 
long int revenue=0;         //revenue
char money_spot[BUF_SIZE]="0";  //money -> dot every three digit variable
int cards[52];              //Card 1 deck Variable
int PlayerCards[4];         //player cards variable
int BankCards[4];           //banker cards variable

int Num_currCard=0;         /* Variable     */
int num_playerCards=0;      /*    for      */
int num_bankCards=0;        /*  hands out cards*/
char BankAddCard;           //Third card added by banker
char PlayerAddCard;         //Third card added by player
int sum_playerCards=0;      //sum of player cards 
int sum_bankCards=0;        //sum of banker cards

int main(int argc,char *argv[]){
    
    int serv_sock;
    struct sockaddr_in  serv_adr;
    pthread_t  menu_thd, create_thd,startGame_thd; 
    int exp,menuItem;

    //exception handling
    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }
    
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    pthread_mutex_init(&mutx,NULL);
    

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    //exception handling
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error\n");
    if(listen(serv_sock,5)==-1)
        error_handling("listen() error\n");
     

    pthread_create(&create_thd,NULL,create_thread,(void*)&serv_sock); //thread create thread
    pthread_create(&startGame_thd,NULL,Start_game,NULL);
  
    pthread_detach(create_thd);
    pthread_detach(startGame_thd);

    menu();
    while(1){
        exp=scanf("%d",&menuItem);
        getchar();

        if(menuItem==0&&exp==1){      //if close server
            fprintf(stderr,"   !!! Good bye !!!");
            break;
        }
        else if(menuItem==1&&exp==1){  //show menu
            menu(); 
        }
        else if(menuItem==2&&exp==1){   //show connected people
            fprintf(stdout,"\n        !!! connected : %d / ready : %d !!!\n",clnt_cnt,ready_user_cnt);
        }
        else{      //exception handling
            fprintf(stdout,"\n!!!Please entered 0 or 1 or 2\n"); 
        }
    }
    
    fprintf(stderr,"\n!!!! Close Server !!!!!\n");
    close(serv_sock);
    return 0;
}

/*clnt_thread create */
void *create_thread(void *arg){

    int serv_sock=*(int *)arg;
    struct sockaddr_in  clnt_adr;
    int clnt_adr_sz;
    int clnt_sock;
    pthread_t clnt_thd;

    while(1){
        if(clnt_cnt<MAX_CLNT){
            clnt_adr_sz=sizeof(clnt_adr);
            clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);

            pthread_mutex_lock(&mutx);
            clnt_cnt++;
            pthread_mutex_unlock(&mutx);
            pthread_create(&clnt_thd,NULL,handle_clnt,(void*)&clnt_sock);  //client handling thread
            pthread_detach(clnt_thd);
        }
    }
}

//client handle thread loutine
void *handle_clnt(void *arg){

    int clnt_sock=*((int*)arg);
    int str_len=0;
    char msg[BUF_SIZE],name_msg[20],money_msg[BUF_SIZE],cardInfo[BUF_SIZE];

    int battingMoney , who_batting;      /*   USER'S */
    char *token;                         /*  batting */
    char *name=malloc(sizeof(char)*20);   /* inforamation */
    
    long int money;  //user's total money

    /* read name, starting money */
    read(clnt_sock,name_msg,sizeof(name_msg));
    read(clnt_sock, money_msg, sizeof(money_msg));
    strcpy(name,name_msg);
    money = atol(money_msg);

    fprintf(stdout,"[%2d/%2d]%s connect...       %s's Money --> %ld\n",clnt_cnt,MAX_CLNT,name,name,money);
    //if ready to start game 
    while((str_len=read(clnt_sock,msg,sizeof(msg)))!=0){
    
        /*read batting information*/
        token = strtok(msg," ");
        who_batting=atoi(token);
        token = strtok(NULL," ");
        battingMoney=atoi(token);

        if(who_batting==0)fprintf(stdout,"%s has bet ￦%d on the banker \n",name,battingMoney);
        else fprintf(stdout,"%s has bet ￦%d on the player \n",name,battingMoney);

        pthread_mutex_lock(&mutx);
        ready_user_cnt++;             //if ready to start game ready_cnt++
        pthread_mutex_unlock(&mutx);
        fprintf(stdout,"connecting user : %d / ready user : %d\n",clnt_cnt,ready_user_cnt);
        
        while(1){
            //When receive a signal form the shuffle thread
            if(gameStart==1){
                //Send game inforamation to the client
                sprintf(cardInfo,"BankerCard : %3d %3d   %c \nPlayerCard : %3d %3d   %c \n\n******** Result ********\n",BankCards[0],BankCards[1],BankAddCard,PlayerCards[0],PlayerCards[1],PlayerAddCard);
                write(clnt_sock,cardInfo,sizeof(cardInfo)); 
                sprintf(cardInfo,"    Banker Sum : %3d\n    Player Sum : %3d\n************************\n",sum_bankCards,sum_playerCards);
                write(clnt_sock,cardInfo,sizeof(cardInfo)) ;

                //If you guess
                if(who_batting==whoWIN){
                    //player win
                    if(who_batting==1){
                        money+=battingMoney;
                        sprintf(cardInfo,"!!!! Cograturation !!!!\n    !!! You Win !!! \nWinning amount : %d\n\n",battingMoney*2);
                        write(clnt_sock,cardInfo,sizeof(cardInfo)); 
                        pthread_mutex_lock(&mutx);
                        revenue-=(battingMoney);
                        pthread_mutex_unlock(&mutx);
                    }
                    //banker win
                    else{
                        money+=(battingMoney*0.95);
                        sprintf(cardInfo,"!!!! Cograturation !!!!\n    !!! You Win !!! \nWinning amount : %d\n\n",(int)(battingMoney*1.95));
                        write(clnt_sock,cardInfo,sizeof(cardInfo)); 
                        pthread_mutex_lock(&mutx);
                        revenue-=(battingMoney*0.95);
                        pthread_mutex_unlock(&mutx);
                    }
                }
                //If wrong
                else {
                    //draw
                    if(whoWIN==2){
                        sprintf(cardInfo,"!!!!!!! Tie !!!!!!!\nWinning amount : %d\n\n",battingMoney);
                        write(clnt_sock,cardInfo,sizeof(cardInfo));
                    }
                    //lose
                    else{
                        money-=battingMoney;
                        sprintf(cardInfo,"   That's sad =D\n   !!! You Lose !!!\n\n");
                        write(clnt_sock,cardInfo,sizeof(cardInfo));
                        pthread_mutex_lock(&mutx);
                        revenue+=battingMoney;
                        pthread_mutex_unlock(&mutx);
                    }
                }

            sprintf(cardInfo,"%ld",money);
            write(clnt_sock,cardInfo,sizeof(cardInfo));  //send to user's left money
            sprintf(msg,"%ld",revenue);
            pthread_mutex_lock(&mutx);
            clarify_number(msg,money_spot);
            pthread_mutex_unlock(&mutx);

        //buffer init
            memset(&msg,0,sizeof(msg));
            memset(&cardInfo,0,sizeof(cardInfo));
            break; //To run once break;
            }
        }
        if(money<10000) break;   // If less than 10,000won , kicked out
    }

    //remove socket(threads)
    pthread_mutex_lock(&mutx);
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    fprintf(stdout,"[%2d/%2d]%s left\n",clnt_cnt,MAX_CLNT,name);
    free(name);
    close(clnt_sock);
    return NULL;
}

void *Start_game(void *arg){
    while(1){
        //All connecting user is ready
        if((clnt_cnt!=0)&&(ready_user_cnt==clnt_cnt)) {
            if(clnt_cnt!=0){
                pthread_mutex_lock(&mutx);
                ready_user_cnt=0;     //ready_cnt init
                shuffle_card();       //shuffle --> hand out cards to rule --> result calculate
                pthread_mutex_unlock(&mutx);
            }
        }
    }
}

//1 deck suffle & bank, player split card 
void shuffle_card(){
            int t;
            int deck[52];
            int cardValue;

            printf("\n******** Game Start ********\n");
            srand(time(NULL));

        //Variable init
            sum_playerCards=0;
            sum_bankCards=0;
            Num_currCard=0;

        // card shuffle
            for(int i=0;i<52;i++){
                cardValue= (i%13)+1; 
                //if J, Q, K 's value is 10
                if(cardValue>10||cardValue==0)deck[i]=10;
                else deck[i]=cardValue;
            }
            for(int i=0;i<52;i++){
                do{ t=rand()%52;
                }while(deck[t]==0);

                if(deck[t]==10) cards[i]=0;
                else cards[i]=deck[t];
                deck[t]=0;
            }

        //Hand out cards    
            BankCards[0]=cards[Num_currCard++];
            PlayerCards[0]=cards[Num_currCard++];
            BankCards[1]=cards[Num_currCard++];
            PlayerCards[1]=cards[Num_currCard++];
            //Additonal cards init to 11
            BankCards[2]=11;
            PlayerCards[2]=11;

            //calculate sum of cards
            for(int i=0;i<2;i++){
                sum_bankCards+=BankCards[i];
                sum_playerCards+=PlayerCards[i];
            }
            //last number is Value
            sum_playerCards%=10;
            sum_bankCards%=10; 
    
            split_card();                   //Additonal card hand out to Rule       
}

//addtiory Card split for Bakara Rule
int split_card(){
    //if Natural
    if((sum_playerCards==8)||(sum_playerCards==9)||(sum_bankCards==8)||(sum_bankCards==9)){
        cal_result(); 
        return 0;
    }
    
    //if player is not stand or natural
    else if(sum_playerCards<6){
        PlayerCards[2]=cards[Num_currCard++];
        sum_playerCards = ( sum_playerCards + PlayerCards[2] ) % 10;
        

        //Banker rule
        if(sum_bankCards<3){
            BankCards[2]=cards[Num_currCard++];
            sum_bankCards=(sum_bankCards+BankCards[2])%10;
        }
        else if(sum_bankCards==3){
            if(PlayerCards[2]==10||PlayerCards[2]==1||PlayerCards[2]==2||PlayerCards[2]==3||PlayerCards[2]==4||PlayerCards[2]==5||PlayerCards[2]==6||PlayerCards[2]==7||PlayerCards[2]==9){
                BankCards[2]=cards[Num_currCard++];
                sum_bankCards=(sum_bankCards+BankCards[2])%10;
            }
        }
        else if(sum_bankCards==4){
            if(PlayerCards[2]==2||PlayerCards[2]==3||PlayerCards[2]==4||PlayerCards[2]==5||PlayerCards[2]==6||PlayerCards[2]==7){
                BankCards[2]=cards[Num_currCard++];
                sum_bankCards=(sum_bankCards+BankCards[2])%10;
            }
        }
        else if(sum_bankCards==5){
            if(PlayerCards[2]==4||PlayerCards[2]==5||PlayerCards[2]==6||PlayerCards[2]==7){
                BankCards[2]=cards[Num_currCard++];
                sum_bankCards=(sum_bankCards+BankCards[2])%10;
            }
        }
        else if(sum_bankCards==6){
            if(PlayerCards[2]==6||PlayerCards[2]==7){
                BankCards[2]=cards[Num_currCard++];
                sum_bankCards=(sum_bankCards+BankCards[2])%10;
            }
        }
    }

    //if player is stand 
    else{
        if(sum_bankCards<6){
            BankCards[2]=cards[Num_currCard++];
            sum_bankCards = ( sum_bankCards + BankCards[2] ) % 10;
    
        }  
    } 
    cal_result();
    return 0;
}

//Who is winner?
void cal_result(){
    char cardInfo[BUF_SIZE];

//Have not received an additonal card init to N
    if(BankCards[2]==11) BankAddCard='N';
    else BankAddCard='0'+BankCards[2];
    if(PlayerCards[2]==11) PlayerAddCard='N';
    else PlayerAddCard='0'+PlayerCards[2];
    
    sprintf(cardInfo,"BankerCard : %3d %3d   %c \nPlayerCard : %3d %3d   %c \n\n********** Result **********\n",BankCards[0],BankCards[1],BankAddCard,PlayerCards[0],PlayerCards[1],PlayerAddCard);
    fprintf(stdout,"%s",cardInfo); 
    sprintf(cardInfo,"    Banker Sum : %3d\n    Player Sum : %3d\n****************************\n",sum_bankCards,sum_playerCards);
    fprintf(stdout,"%s",cardInfo); 

    //calculate winner
    if(sum_bankCards>sum_playerCards) {
        whoWIN = 0;                             //banker win
        fprintf(stdout,"        Banker is win\n");
    }
    else if(sum_bankCards<sum_playerCards){ 
        whoWIN = 1;                             //plyaer win
        fprintf(stdout,"        Player is win\n");
    }
    else {
        whoWIN = 2;                              //Tie
        fprintf(stdout,"           Tie\n");
    }
    printf("****************************\n");
    gameStart=1;                        //signal to clnt_thread 
    sleep(1);
    gameStart=0;                     //Reset game start signal
    


}

//menu board
void menu(){
    system("clear");
    fprintf(stdout,"************ Bakara Game Server ***********\n\n");
    fprintf(stdout,"            Revenue : %s \n",money_spot);    
    fprintf(stdout,"\n****************** Menu *******************\n\n");
    fprintf(stdout,"             0.close server \n");
    fprintf(stdout,"                1. clear \n");
    fprintf(stdout,"       2. show number of participant \n");
    fprintf(stdout," \n*******************************************\n\n");
}    

void clarify_number(const char* str, char* buf){
    while (*buf++ = *str++) {
        if ((strlen(str) % 3) == 0) *buf++ = (*str ? ',' : '\0');
    }
}

void error_handling(char *msg){
    fputs(msg,stderr);
    fputc('n',stderr);
    exit(1);
}
