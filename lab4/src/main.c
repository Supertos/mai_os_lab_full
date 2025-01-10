#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>

typedef struct Allocator Allocator;
typedef Allocator* (*allocator_create_fn)(void *memory, size_t size);
typedef void (*allocator_destroy_fn)(Allocator *allocator);
typedef void* (*allocator_alloc_fn)(Allocator *allocator, size_t size);
typedef void (*allocator_free_fn)(Allocator *allocator, void *memory);

Allocator *standard_allocator_create(void *memory, size_t size) {
    (void)size;    
    (void)memory;    
    return memory;
}

void *standard_allocator_alloc(Allocator *allocator, size_t size) {    
    (void)allocator;    
    size_t *memory = mmap(NULL, size + sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);    
    if (memory == MAP_FAILED) {        
        return NULL;    
    }    
    *memory = (size_t)(size + sizeof(size_t));
    return memory + 1;
}

void standard_allocator_free(Allocator *allocator, void *memory) {    
    (void)allocator;    
    if (memory == NULL) 
        return;    
    size_t *mem = (size_t *)memory - 1;    
    munmap(mem, *mem);
}


void standard_allocator_destroy(Allocator *allocator) { (void)allocator; }


void testAllocator( allocator_create_fn create, allocator_destroy_fn destroy, allocator_alloc_fn cmalloc, allocator_free_fn cfree, char* name ) {
	printf( "===== %s =====\n", name );
	void* memory;
	
	double avgMemoryUsable = 0;
	double probes = 0;
	
	
	struct timeval begin;
	gettimeofday( &begin, NULL );
	
	double time_taken;
	
	
	int def = !strcmp( name, "DEFAULT" );
	
	for( size_t i = 128; i < 32768 * 8; i *= 2 ) {
		memory = mmap(NULL, i, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		Allocator* alloc = create( memory, i );
		
		double bytesAllocated = 0;
		
		while( (!def || bytesAllocated < 8192 ) && cmalloc( alloc, 16 ) ) bytesAllocated += 16;
		// printf( "DEBUG: %zu %.2f \n", i, bytesAllocated );
		
		avgMemoryUsable += bytesAllocated / i;
		
		
		destroy( alloc );
		munmap(memory, i);
		
		probes++;
	}
	struct timeval end; 
	gettimeofday( &end, NULL );
	
    time_taken = (end.tv_sec - begin.tv_sec);
    time_taken += (end.tv_usec - begin.tv_usec) * 1e-6;
	
	printf( "Average Memory Efficiency: %f\n", avgMemoryUsable / probes * 100 );
	printf( "Allocation speed test (Different sizes, full drain): %f\n", time_taken );
	
	
	
	
	
	memory = mmap(NULL, 32768 * 1024 * 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
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
	
	
	munmap(memory, 32768 * 1024 * 4);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
		testAllocator( standard_allocator_create, standard_allocator_destroy, standard_allocator_alloc, standard_allocator_free, "DEFAULT" );
		
    }else{

		void *handle = dlopen(argv[1], RTLD_NOW);
		if (!handle) {
			testAllocator( standard_allocator_create, standard_allocator_destroy, standard_allocator_alloc, standard_allocator_free, "DEFAULT" );
		}else{
			allocator_create_fn allocator_create = (allocator_create_fn)dlsym(handle, "allocator_create");
			allocator_destroy_fn allocator_destroy = (allocator_destroy_fn)dlsym(handle, "allocator_destroy");
			allocator_alloc_fn allocator_alloc = (allocator_alloc_fn)dlsym(handle, "allocator_alloc");
			allocator_free_fn allocator_free = (allocator_free_fn)dlsym(handle, "allocator_free");

			if (!allocator_create || !allocator_destroy || !allocator_alloc || !allocator_free) {
				dlclose(handle);
				testAllocator( standard_allocator_create, standard_allocator_destroy, standard_allocator_alloc, standard_allocator_free, "DEFAULT" );
				return 1;
			}else{
				testAllocator( allocator_create, allocator_destroy, allocator_alloc, allocator_free, argv[1] );
				
				dlclose(handle);
				return 0;
				
			}
			
		}
	}
}