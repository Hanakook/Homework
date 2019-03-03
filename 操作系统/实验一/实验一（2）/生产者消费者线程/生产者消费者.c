#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXSIZE 3
#define SLEEPTIME 3

sem_t semF, semE;

void* producer( void* arg )
{
	for ( ;; ) {
		if ( sem_wait( &semE ) == 0 ) { //  p操作成功
			printf( "produce\n" );
			sem_post( &semF );
		}
		sleep( rand()%SLEEPTIME+1 );
	}

}

void* consumer( void* arg )
{
	for ( ;; ) {
		if ( sem_wait( &semF ) == 0 ) {
			printf( "consume\n" );
			sem_post( &semE );
		}
		sleep( rand()%SLEEPTIME+2 );
	}
}

int main()
{
	sem_init( &semE, 0, MAXSIZE );
	sem_init( &semF, 0, 0 );

	pthread_t pth1, pth2;

	pthread_create( &pth1, NULL, producer, NULL );
	pthread_create( &pth2, NULL, consumer, NULL );
	
	printf( "press ctrl+c to exit\n" );
	pthread_join( pth1, NULL );
	pthread_join( pth2, NULL );

	sem_destroy( &semE );
	sem_destroy( &semF );

	return 0;
}

