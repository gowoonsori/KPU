/*2020.05.26  complete  bakaragame-client
        2015152042 Hong-eui-sung

 Baccarat game code using multi-thread and mutex

 Features to add in the future - side batting( Tie / Pair)
                               - all input exception handling
                               - code optimization
                                                                                */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>
 
#define BUF_SIZE 100
#define NAME_BUF_SIZE 20
 
void* connect_server(void* arg);
void error_handling(char* msg);
void menu();                       //menu board
void GameOut();                   //game out menu board
void clarify_number(const char* str, char* buf);    //money -> dot every three digit method
 
char name[NAME_BUF_SIZE]="[DEFALT]";     // name
long int money;                       //Starting money
char money_msg[BUF_SIZE];             
char money_spot[BUF_SIZE];              //money -> dot every three digit
 
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread;
    void* thread_return;

    if (argc!=5)
    {
        fprintf(stderr," Usage : %s <ip> <port> <name> <money>\n", argv[0]);
        exit(1);
    }
 
    sprintf(name, "[%s]", argv[3]);
    sprintf(money_msg,"%s",argv[4]);
    clarify_number(money_msg,money_spot); //money -> dot every three digit
    money=atol(money_msg);

    if(money<10000){
        error_handling("!!! Please enter more than 10,000 won !!!\n");
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
 
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" conncet() error\n");
 
    /** call menu **/
    system("clear");
    menu();
 
    pthread_create(&snd_thread, NULL, connect_server, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    close(sock);
    return 0;
}
 
void* connect_server(void* arg)
{
    int sock=*((int*)arg);
    char msg[BUF_SIZE],myInfo[BUF_SIZE];
    char join;   //yes or no
    int str_len;
    int scan_excep;  //variable for scanf exception

    int who_batting,batting_money; //user's batting inf


    /*user's name , start money write*/
    write(sock,name,sizeof(name));      
    write(sock,money_msg, sizeof(money_msg));

    while(1)
    {
        /*Asking  game start */
        fprintf(stdout,"\n  >> join the game ?  \n");
        fprintf(stdout," Join : y(Y)  / Quit : q(Q)\n");
        scanf("%c",&join);
        getchar();

        if(join=='y'||join=='Y'){
            do{
                fprintf(stdout,"Who would  you like to bet on?\n");
                scan_excep=scanf("%d",&who_batting); //if input type is int, scan_excep is 1 
                getchar();
            }while(!((who_batting==1||who_batting==0)&&scan_excep==1));   //1 or 0

            do{
                fprintf(stdout,"Hou much will you bet on?\n");
                scan_excep=scanf("%d",&batting_money);
                getchar();
            }while(!((batting_money>=10000&&batting_money<=money&&batting_money<=10000000)&&scan_excep==1));   //  10,000<= batting money <=10,000,000/[getting money]


            /*batting information write*/
            sprintf(myInfo, "%d %d",who_batting,batting_money);
            write(sock, myInfo, strlen(myInfo));
            

            fprintf(stdout,"\nwaitting to ready..........\n\n\n");

            for(int i=0;i<4;i++){
                str_len=read(sock, msg, sizeof(msg));
                msg[str_len]=0;
                if(i!=3)fputs(msg, stdout);  //money msg need not puts
            }
            clarify_number(msg,money_spot);
            money=atol(msg);
        }
 
        else if(join=='q'||join=='Q'){  
            return NULL;
        }
        
        else{
            printf("Incorrect answer!!\n");
            fflush(stdin);
            continue;
        }

        sleep(1);  //waiting game result for tension

        if(money>=10000) menu();    //if more than 10,000won
        else{ GameOut(); break;}    //if less than 10,000won, kicked out
    }
    return NULL;
}


void menu(){

    fprintf(stdout,"\n\n************ Welcom Bakara Game ***********\n");
    fprintf(stdout,"                your name   : %s \n", name);
    fprintf(stdout,"           your total money : %s \n", money_spot);
    fprintf(stdout,"************** Batting Rule ***************\n\n");
    fprintf(stdout,"!!! The minumum bet amount is 10,000won !!!\n\n");
    fprintf(stdout,"! The maximum bet amount is 10,000,000won !\n\n");
    fprintf(stdout,"************* Batting Number **************\n\n");
    fprintf(stdout,"               Banker  : 0\n");
    fprintf(stdout,"               Player  : 1\n");
    fprintf(stdout," \n*******************************************\n\n");
}    

void GameOut(){
    system("clear");
    fprintf(stdout," *********** Welcom Bakara Game ***********\n");
    fprintf(stdout,"                your name   : %s \n", name);
    fprintf(stdout,"           your total money : %ld \n", money);
    fprintf(stdout,"*******************************************\n\n");
    fprintf(stdout,"          You don't have enough money\n");
    fprintf(stdout,"\n*******************************************\n\n");
    fprintf(stdout,"        ┌────┐      │   │      ┌────\n");
    fprintf(stdout,"        │    │      │   │      │      \n"); 
    fprintf(stdout,"        │───┤       └─┬─┘      ├────\n");
    fprintf(stdout,"        │    │        │        │      \n"); 
    fprintf(stdout,"        └────┘        │        └────\n");
    fprintf(stdout,"\n*******************************************\n\n");
}    
 
void clarify_number(const char* str, char* buf){
    while (*buf++ = *str++) {
        if ((strlen(str) % 3) == 0) *buf++ = (*str ? ',' : '\0');
    }
}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
