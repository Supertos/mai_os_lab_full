#define _GNU_SOURCE

#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

#include "commandline.h"
#include "utils.h"

#define CLIENT_NAME "/client.out"
#define SHM_NAME "/memA"
#define SEM_NAMEA "/semA"
#define SEM_NAMEB "/semB"

#define ERR_CANT_INIT_SEM 777
#define ERR_CANT_INIT_SHM 888

#define ERR_CANT_OPEN_SEM 999
#define ERR_CANT_OPEN_SHM 1111

struct ProgramState {
    int dummy;
};

char* getexecpath() {
    char* path = malloc(MAX_PATH_LEN * 2);
    size_t len = readlink("/proc/self/exe", path, MAX_PATH_LEN);
    
    if (!len) {
        free(path);
        return NULL;
    }
    
    while (path[len] != '/') len--;
    path[len] = '\0';
    
    return path;
}

char* readpath(int argc, char** argv) {
    CMD* command = initCMD(argc, argv);
    if (!command) return NULL;
    
    char* path = expectCMD(command, String);
    freeCMD(command);
    
    return path;
}

static inline int readchar( int file, char* trg ) {
	return read( file, trg, 1 );
}

static inline bool strempty( char* buf ) {
	return buf[0] == '\0';
}

int readuint(const int file, char* buf, size_t n) {
    char c;
    bool foundint = false;
    int readstatus;
    while ((readstatus = readchar(file, &c))) {
        if (readstatus == -1) return ERR_INVALID_INPUT;
        if (IS_BLANK(c)) 
            if (foundint) break; else continue;
        
        if (!IS_DIGIT(c)) return ERR_INVALID_INPUT;
        foundint = true;
        
        *(buf++) = c;
        n--;
        if (!n) return ERR_INVALID_INPUT;
    }
    *(buf++) = '\0';
    
    return NO_ERR;
}

int main(int argc, char** argv) {
    char* path;
    if (!(path = getexecpath())) return error(ERR_PATH_READ);
    
    char* target;
    if (!(target = readpath(argc, argv))) return error(ERR_INVALID_INPUT);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) return error(ERR_CANT_INIT_SHM);
    
    if (ftruncate(shm_fd, MAX_NUM_LEN) == -1) return error(ERR_CANT_INIT_SHM);
    
    char* shared_mem = mmap(NULL, MAX_NUM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) return error(ERR_CANT_INIT_SHM);
    
    sem_unlink(SEM_NAMEA);
    sem_unlink(SEM_NAMEB);
    sem_t* semA = sem_open(SEM_NAMEA, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (semA == SEM_FAILED) return error(ERR_CANT_INIT_SEM);
    sem_t* semB = sem_open(SEM_NAMEB, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (semB == SEM_FAILED) return error(ERR_CANT_INIT_SEM);
    
    int exitcode = 0;
    switch (fork()) {
        case -1:
            exitcode = ERR_CANT_INIT_CHILD;
            break;
        case 0: {
            char *const args[] = { CLIENT_NAME, NULL };
            strcpy(&path[strlen(path)], CLIENT_NAME);
            exitcode = execv(path, args);
            break;
        }
        default: {
            free(path);
            close(shm_fd);
            
            int file;
            if ((file = open(target, O_RDONLY)) == -1) return error(ERR_NO_SUCH_FILE);
            
            char buf[MAX_NUM_LEN];
            int readerr;
            while (!(readerr = readuint(file, buf, MAX_NUM_LEN)) && !strempty(buf)) {
				strcpy( shared_mem, buf );
                sem_post(semA);
                sem_wait(semB);		
				while( shared_mem[0] != 0 && shared_mem[0] != 1 );
				if( shared_mem[0] == 1 ) {
					break;
				}else{
					write( stdout, buf, strlen(buf) );
					write( stdout, "\n", 1 );
				}			
            }
            sem_post(semA);
            
            close(file);
            
            int childStatus;
            wait(&childStatus);
            if (!WIFEXITED(childStatus)) exitcode = childStatus;
            exitcode |= readerr;
            break;
        }
    }

    sem_close(semA);
    sem_close(semB);
    sem_unlink(SEM_NAMEA);
    sem_unlink(SEM_NAMEB);
    munmap(shared_mem, MAX_NUM_LEN);
    shm_unlink(SHM_NAME);

    if (exitcode) return error(exitcode);
} 