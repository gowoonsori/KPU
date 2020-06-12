#include <sys/types.h>
#include <sys/times.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_NUMBER 10000    //약수를 구할 수
#define MAX_THREADS 10000  //최대 프로세스 개수
#define CLK_TCK sysconf(_SC_CLK_TCK)

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  //동기화를 위한 mutex

void* thread_func( void* arg );            //쓰레드 함수
static unsigned int shared_Value = 0;     //약수 개수
static unsigned int n_Threads;           //스레드 개수
int loopValue=1;                         //루프 변수

int main( int argc, char** argv )
{
    n_Threads = atoi( argv[1] );  //생성할 스레드 개수

    pthread_t tid[MAX_THREADS];  //입력받은 만큼 스레드 변수선언

    /*Time setup*/
    time_t t0;
    struct tms mytms;
    clock_t t1, t2;

    /*측정 시작*/
    if( (t1 = times( &mytms )) == -1 )
    {
        perror( "times 1" );
        exit( 1 );
    }

    /*최대 스레드 개수 만큼 스레드 생성*/
    for( int i = 0; i < n_Threads; i++ )
    {
        if( pthread_create( &tid[i], NULL, &thread_func, NULL ) != 0 ){
            fprintf( stderr, "pthread_create error!\n" );
            fprintf( stderr, "%s\n", strerror( errno ) );
            exit( 1 );
        }
    }

    /*모든 스레드가 종료될때 까지 join*/
    for( int i = 0; i < n_Threads; i++ )
    {
        pthread_join( tid[i], NULL );
    }

    /*측정 종료*/
    if( (t2 = times( &mytms )) == -1 )
    {
        perror( "times 2" );
        exit( 1 );
    }

    fprintf( stdout, "%d 의  약 수 의  개 수 : %d \n", MAX_NUMBER, shared_Value );

    /*측정한 시간 printf*/
    printf( "Real time : %.5f sec\n", (double)(t2 - t1) / CLK_TCK );
    printf( "User time : %.5f sec\n", (double)mytms.tms_utime / CLK_TCK );
    printf( "System time : %.5f sec\n", (double)mytms.tms_stime / CLK_TCK );

    return 0;
}

void* thread_func( void* arg )
{

    /*약수를 구할 수만큼 1부터 roof*/
    while(loopValue<=MAX_NUMBER){

        pthread_mutex_lock( (&mutex) );
        /*약수가 맞다면 약수 개수 증가*/
        if( MAX_NUMBER % loopValue == 0 ) {
            shared_Value++;
        }
        loopValue++;
        pthread_mutex_unlock( (&mutex) );
    }
    return NULL;
}

/*
추가 사항 : 쓰레드 함수에서 연산의 범위를 100000개, 쓰레드 개수를 100개 이상으로 설정시, 
pthread_create에서 resource temporarily unavailable 발생함.
이는, 다수의 쓰레드가 1부터 100000까지 연산을 수행하면서 join 되지 않고 계속 쌓이게 되고
자원 공간이 꽉 차서 resource 관련 에러가 나지 않나 싶음.
*/