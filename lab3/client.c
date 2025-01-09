#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

#include "commandline.h"
#include "utils.h"
#include "number.h"


#define SHM_NAME "/memA"
#define SEM_NAMEA "/semA"
#define SEM_NAMEB "/semB"

#define ERR_CANT_OPEN_SEM 999
#define ERR_CANT_OPEN_SHM 1111

int isPrime(double a) {
    for (size_t i = 2; i <= sqrt(a); i++) 
        if ((size_t)a % i == 0) return 0;
    
    return 1;
}
static inline int readchar( int file, char* trg ) {
	return read( file, trg, 1 );
}

static inline bool strempty( char* buf ) {
	return buf[0] == '\0';
}

int main(int argc, char** argv) {
    sem_t* semA = sem_open(SEM_NAMEA, 0);
    if (semA == SEM_FAILED) return error(ERR_CANT_OPEN_SEM);
    sem_t* semB = sem_open(SEM_NAMEB, 0);
    if (semB == SEM_FAILED) return error(ERR_CANT_OPEN_SEM);

    int shm_fd = shm_open(SHM_NAME, O_RDWR , 0666);
    if (shm_fd == -1) return error(ERR_CANT_OPEN_SHM);
    
    char* shared_mem = mmap(NULL, MAX_NUM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) return error(ERR_CANT_OPEN_SHM);
    
    char buffer[MAX_NUM_LEN + 1];
    while (1) {
        sem_wait(semA);
		
        strcpy(buffer, shared_mem);
		if( cstrtod(buffer) < 0 || isPrime(cstrtod(buffer)) ) {
			shared_mem[0] = 1;
			sem_post(semB);
			break;
		}else{
			shared_mem[0] = 0;
			sem_post(semB);
		}
		
        if (strempty(buffer)) break;
        
    }

    sem_close(semA);
    sem_close(semB);
    munmap(shared_mem, MAX_NUM_LEN);
    close(shm_fd);

    return 0;
}