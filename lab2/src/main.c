/* Supertos Industries ( 2012 - 2024 ) 
 * Author: Supertos, 2024
 *
 * MAI Operating Systems course. Exercise 2.
 */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include <sys/time.h>


typedef struct ArrayPassInfo ArrayPassInfo;
struct ArrayPassInfo {
	size_t mask;
	size_t offset;
	size_t dist;
	size_t start;
	size_t end;
	size_t arrSize;
	int* arr;
};

void swapInt( int* a, int* b ) {
	int temp = *b;
	*b = *a;
	*a = temp;
}

/* One array pass */
void* sortArrayPassFraction( void* infoP ) {
	if( !infoP ) return NULL;
	
	ArrayPassInfo* info = (ArrayPassInfo*)infoP;
	
	size_t mask = info->mask;
	size_t offset = info->offset;
	size_t dist = info->dist;
	size_t arrSize = info->arrSize;
	size_t start = info->start;
	size_t end = info->end;
	int* arr = info->arr;
	
	
	for( size_t i = start; i < end; ++i ) {
		if( ( i & mask ) != offset ) continue; // Skip elements that are in even or odd blocks, to avoid overlapping.
		if( i + dist < arrSize && arr[i] > arr[i + dist] ) swapInt( &arr[i], &arr[i + dist] ); 
	}
	
	return NULL;
}

double sortArrayPass( size_t mask, size_t offset, size_t dist, size_t threads, int arr[], size_t arrSize, size_t realArrSize, ArrayPassInfo* infoTable, pthread_t* threadlist ) {
		
    size_t elementsLeft = arrSize - dist;
    size_t start = 0;
		
	for( size_t threadNo = threads; threadNo > 0; --threadNo ) {
		size_t elements = elementsLeft / threadNo;
			
		elementsLeft -= elements;
			
		infoTable[threadNo - 1] = (ArrayPassInfo){
			.mask = mask,
			.offset = offset,
			.dist = dist,
				
			.start = start,
			.end = start + elements,
			.arrSize = realArrSize,
			.arr = arr
		};
		
		pthread_create( &threadlist[threadNo - 1], NULL, sortArrayPassFraction, &infoTable[threadNo - 1] );
		start += elements;
	}
	
	struct timeval begin;
	gettimeofday( &begin, NULL );
	for( size_t threadNo = threads; threadNo > 0; --threadNo ) {
		pthread_join( threadlist[threadNo - 1], NULL );
	}
	struct timeval end; 
	gettimeofday( &end, NULL );
	
	double time_taken;
	
    time_taken = (end.tv_sec - begin.tv_sec);
    time_taken += (end.tv_usec - begin.tv_usec) * 1e-6;
	return time_taken;
}

double batcherSort( int arr[], size_t size, size_t threads, ArrayPassInfo* infoTable, pthread_t* threadlist ) {
	size_t batcherSize = pow( 2, ceil(log2( size )));
	
	double procTime = 0;
	
	for (int mask = batcherSize; mask > 0; mask /= 2) {
		procTime += sortArrayPass( mask, 0, mask, threads, arr, batcherSize, size, infoTable, threadlist );
		procTime += sortArrayPass( mask, mask, batcherSize - mask, threads, arr, batcherSize, size, infoTable, threadlist );
    }
	
	return procTime;
}

int* generateRandomArray(int size) {
    int* arr = (int*)malloc(sizeof(int) * size);
    if (arr == NULL) {
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        arr[i] = rand() / ( RAND_MAX / 2 );
    }
 
    return arr;
}
int* generateBitonicSequence(int size) {
    if (size <= 0) return NULL;
    int* arr = (int*)malloc(sizeof(int) * size);
    if (arr == NULL) return NULL;

    int mid = size / 2;
    int current = rand() % 100;
    
    for(int i = 0; i < mid; ++i) {
        arr[i] = current;
        current += rand() % 3;
    }

    for(int i = mid; i < size; ++i) {
        arr[i] = current;
        current -= rand() % 3;
        if(current < 0)
        {
            current = 0;
        }
    }
    return arr;
}

int main( size_t argc, char** argv ) {
    srand(time(NULL));
	
	size_t tot = 50;
	
	size_t threads;
	sscanf( argv[1], "%zu", &threads );
	printf( "Threads: %zu...", threads );
	
	int* arr = generateBitonicSequence(32768 * 8);
	printf( "Array Generated!\n" );
	
	
	ArrayPassInfo* infoTable = (ArrayPassInfo*)malloc( sizeof( ArrayPassInfo ) * threads );
	pthread_t* threadlist = (pthread_t*)malloc( sizeof( pthread_t ) * threads );
	
	
	double procTime = 0;
	
	for( size_t i = 0; i < tot; ++i ) {
		double ms = batcherSort( arr, 32768 * 8, threads, infoTable, threadlist );
		procTime += ms;
	}
	free( arr );
	free( infoTable );
	
	
	printf( "Tot time: %lf s\n", procTime );
	printf( "Avg time: %lf s\n", procTime / (double)tot );

	return 0;
}


