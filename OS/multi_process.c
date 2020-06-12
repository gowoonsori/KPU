#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>      
#include <fcntl.h>

#define BUF_SIZE 32 //공유 메모리 사이즈
#define SHM_KEY 0x1234 //공유 메모리 식별 key

#define MAX_NUMBER 10000    //약수를 구할 수
#define MAX_PROCESS 10000  //최대 프로세스 개수
#define CLK_TCK sysconf(_SC_CLK_TCK)

int main( int argc, char** argv )
{
    int n_Process = atoi( argv[1] ); //생성할 프로세스 개수

    int shmid; //공유메모리 접근을 위한 id
    int* shmaddr; //공유메모리 변수
    sem_t* sem; //동기화를 위한 세마포어 변수
    unsigned int value = 1; //세마포어에 접근가능한 초기 값
    pid_t pids[MAX_PROCESS]; //입력한 프로세스 개수만큼 프로세스 변수선언

    /* Time setup */
    time_t t0;
    struct tms mytms;
    clock_t t1, t2;

    /*측정 시작*/
    if( (t1 = times( &mytms )) == -1 )
    {
        perror( "times 1" );
        exit( 1 );
    }

    shmid = shmget( SHM_KEY, BUFSIZ, IPC_CREAT | 0644 ); //공유메모리 공간 할당
    if( shmid == -1 )
    {
        perror( "shmget" );
        exit( 1 );
    }

    shmaddr = (int*)shmat( shmid, (int*)NULL, 0 ); //공유메모리 생성

    shmaddr[0] = 0;   //약수개수를 저장
    shmaddr[1] = 1;   //반복문을 위한 변수

    /*프로세스간 세마포어에 접근할 수있게 할당 */
    sem = sem_open( "pSem", O_CREAT | O_EXCL, 0644, value ); //pSem 의 이름의 세마포어 생성,같은 이름의 세마포어가 있다면 수행x.

    /* fork child processes */
    for( int i = 0; i < n_Process; i++ )
    {
        if( (pids[i] = fork()) < 0 )
        {
            /* pid 자식프로세스가 fork error 났다면 */
            sem_unlink( "pSem" ); //psem의 세마포어 삭제 
            sem_close( sem ); // 세마포어 공간할당 해제
            //fprintf( stderr, "Fork() error.\n" );
            printf( "problem with fork() in pid %d error %s!!! \n",
                (int)getpid(), strerror( errno ) );
        }
        /*Child process*/
        else if( pids[i] == 0 )
        {               
            while(shmaddr[1] <= MAX_NUMBER){
                sem_wait( sem ); //세마포어를 얻을때 까지 기다림,들어가면 lock
                if( MAX_NUMBER % shmaddr[1] == 0 )
                {
                    shmaddr[0]++;   //약수 개수 증가
                }
                shmaddr[1]++;     //반복문 변수 증가
                sem_post( sem ); //세마포어 잠금 해제 
            }
            exit( 0 );
        }
        int status;
        pid_t pid;
        pid = wait( &status ); //자식프로세스가 끝날때까지 wait
    }

    /*측정 종료*/
    if( (t2 = times( &mytms )) == -1 )
    {
        perror( "times 2" );
        exit( 1 );
    }
    fprintf( stdout, "%d 의  약 수 의  개 수 : %d \n", MAX_NUMBER, shmaddr[0] );

    printf( "Real time : %.5f sec\n", (double)(t2 - t1) / CLK_TCK );
    printf( "User time : %.5f sec\n", (double)mytms.tms_utime / CLK_TCK );
    printf( "System time : %.5f sec\n", (double)mytms.tms_stime / CLK_TCK );

    /* 측정한 시간 printf */
    shmdt( shmaddr );
    shmctl( shmid, IPC_RMID, 0 );

    sem_unlink( "pSem" ); //pSem이름의 세마포어 제거     
    sem_close( sem ); //세마포어 공간 해제

    exit( 0 );

}

/* Referene */
// https://12bme.tistory.com/227
// https://awesomebit.tistory.com/19
// https://stackoverflow.com/questions/16400820/how-to-use-posix-semaphores-on-forked-processes-in-c
// https://www.joinc.co.kr/w/man/2/waitpid
// https://stackoverflow.com/questions/876605/multiple-child-process

/*
추가 사항 
86~88번줄에서 wait으로 자식 프로세스가 종료되기를 기다리는데, 이를 for문 밖으로 꺼낼 경우,
자식 프로세스가 끝났음에도 변경 상태를 회수해 가지 않아서 프로세스 슬롯을 차지하게 됨. (좀비 프로세스)
그래서 종료 되었음에도 자원을 소비하여 fork 시에 resource 관련 에러가 생김.
따라서, 자식 프로세스가 exit(0)으로 종료되자마자 회수하여 이를 방지함.

참고 내역 : https://stackoverflow.com/questions/28140355/c-fork-resource-temporarily-unavailable-on-tcp-server
*/