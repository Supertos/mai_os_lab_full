#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <sys/time.h>

typedef struct Allocator Allocator;
typedef Allocator* (*allocator_create_fn)(void *memory, size_t size);
typedef void (*allocator_destroy_fn)(Allocator *allocator);
typedef void* (*allocator_alloc_fn)(Allocator *allocator, size_t size);
typedef void (*allocator_free_fn)(Allocator *allocator, void *memory);


void testAllocator( allocator_create_fn create, allocator_destroy_fn destroy, allocator_alloc_fn cmalloc, allocator_free_fn cfree, char* name ) {
	printf( "===== %s =====\n", name );
	
	
	
	void* memory;
	
	double avgMemoryUsable = 0;
	double probes = 0;
	
	
	struct timeval begin;
	gettimeofday( &begin, NULL );
	
	double time_taken;
	
	for( size_t i = 128; i < 32768 * 8; i *= 2 ) {
		memory = malloc( i );
		Allocator* alloc = create( memory, i );
		
		double bytesAllocated = 0;
		
		while( cmalloc( alloc, 16 ) ) bytesAllocated += 16;
		// printf( "DEBUG: %zu %.2f \n", i, bytesAllocated );
		
		avgMemoryUsable += bytesAllocated / i;
		
		
		destroy( alloc );
		free( memory );
		
		probes++;
	}
	struct timeval end; 
	gettimeofday( &end, NULL );
	
    time_taken = (end.tv_sec - begin.tv_sec);
    time_taken += (end.tv_usec - begin.tv_usec) * 1e-6;
	
	printf( "Average Memory Efficiency: %f\n", avgMemoryUsable / probes * 100 );
	printf( "Allocation speed test (Different sizes, full drain): %f\n", time_taken );
	
	
	
	
	
	memory = malloc( 32768 * 1024 * 4 );
	if( !memory ) printf( "No mem :(\n" );
	Allocator* alloc = create( memory, 32768 * 1024 * 4 );
	
	void* allocated[10000];
	size_t i = 0;
	while( i < 10000 && (allocated[i++]=cmalloc( alloc, 16 )) );
		
	gettimeofday( &begin, NULL );
	for( size_t i = 0; i < 10000; i++ ) {
		
		cfree( alloc, allocated[i] );
	}
	gettimeofday( &end, NULL );
	
    time_taken = (end.tv_sec - begin.tv_sec);
    time_taken += (end.tv_usec - begin.tv_usec) * 1e-6;
	printf( "Free speed test (One size, 10K elements): %f\n", time_taken );
	printf( "===== === =====\n" );
	
	
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No allocator library specified.\n");
        return 1;
    }

    void *handle = dlopen(argv[1], RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return 1;
    }

    allocator_create_fn allocator_create = (allocator_create_fn)dlsym(handle, "allocator_create");
    allocator_destroy_fn allocator_destroy = (allocator_destroy_fn)dlsym(handle, "allocator_destroy");
    allocator_alloc_fn allocator_alloc = (allocator_alloc_fn)dlsym(handle, "allocator_alloc");
    allocator_free_fn allocator_free = (allocator_free_fn)dlsym(handle, "allocator_free");

    if (!allocator_create || !allocator_destroy || !allocator_alloc || !allocator_free) {
        fprintf(stderr, "Error loading allocator functions.\n");
        dlclose(handle);
        return 1;
    }
	
	testAllocator( allocator_create, allocator_destroy, allocator_alloc, allocator_free, argv[1] );
	
    dlclose(handle);
    return 0;
}