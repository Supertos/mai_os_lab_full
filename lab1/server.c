// -=-=-=-=-=-=-=-=-=-= Supertos Industries ( 2012 - 2024 ) =-=-=-=-=-=-=-=-=-=-
// Author; Supertos
// 
// Exercise 1
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#define _GNU_SOURCE

#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "commandline.h"
#include "utils.h"

/* Custom defines start here */
#define CLIENT_NAME "/client.out"

struct ProgramState { // Empty structures unallowed, see C99 spec.
	int dummy;
};

/* Returns this executable directory */
char* getexecpath() {
	char* path = malloc( MAX_PATH_LEN * 2 );
	size_t len = readlink( "/proc/self/exe", path, MAX_PATH_LEN );
	
	if( !len ) {
		free( path );
		return NULL;
	}
	
	while( path[len] != '/' ) len--;
	path[len] = '\0';
	
	return path;
}

/* Reads path from stdin */
char* readpath( int argc, char** argv ) {	
	CMD* command = initCMD( argc, argv );
	if( !command ) return NULL;
	
	char* path = expectCMD( command, /* CMDType */ String );
	freeCMD( command );
	
	return path;
}

/* Setups pipe; overrides [override] with pipe [end] and closes the other [end] */
void setpipe( int* pipe, size_t end, size_t override ) {
	dup2( pipe[end], override );
	close( pipe[!end] ); // Since pipe is always bi-directional
}

/* Reads single character from [file] to [trg] */
static inline int readchar( int file, char* trg ) {
	return read( file, trg, 1 );
}

/* Returns true if [buf] is empty */
static inline bool strempty( char* buf ) {
	return buf[0] == '\0';
}

/* Reads unsigned int from [file] to [buf] of size [n] */
int readuint( const int file, char* buf, size_t n ) {
	char c;
	bool foundint = false;
	int readstatus;
	while( (readstatus=readchar( file, &c )) ) {
		if( readstatus == -1 ) return ERR_INVALID_INPUT;
		if( IS_BLANK(c) ) 
			if( foundint ) break; else continue;
		
		if( !IS_DIGIT(c) ) return ERR_INVALID_INPUT;
		foundint = true;
		
		*(buf++) = c;
		n--;
		if( !n ) return ERR_INVALID_INPUT;
	}
	*(buf++) = '\0';
	
	return NO_ERR;
}

/* Entry point */
int main( int argc, char** argv ) {
	char* path;
	if( !(path=getexecpath()) ) return error( ERR_PATH_READ );
	
	char* target;
	if( !(target=readpath( argc, argv )) ) return error( ERR_INVALID_INPUT );
	
	int childPipe[2];
	if( pipe2( childPipe, O_DIRECT ) ) return error( ERR_CANT_INIT_PIPE );

	int exitcode = 0;
	switch( fork() ) {
		case -1: // An error occured
			exitcode = ERR_CANT_INIT_CHILD;
		        break;
		case 0: // We're child
			setpipe( childPipe, 0, stdin );
			
			char *const args[] = { CLIENT_NAME, NULL };
			strcpy( &path[strlen(path)], CLIENT_NAME );
			
			exitcode = execv( path, args );
		        break;
		default: // We're parent
			free( path ) // Don't forget about this one
			close( childPipe[0] ); // It is imperative we close the other end AND preserve out stdin/stdout
			
			int file;
			if( (file=open( target, 0, "r" )) == -1 ) return error( ERR_NO_SUCH_FILE );
			
			char buf[MAX_NUM_LEN];
			int readerr;
			while( !(readerr=readuint( file, buf, MAX_NUM_LEN )) && !strempty(buf) )
				write( childPipe[1], buf, strlen(buf) );
			
			close( childPipe[1] );
			close( file );
			
			int childStatus;
			wait(&childStatus);
			if( !WIFEXITED( childStatus ) ) exitcode = childStatus;
			exitcode |= readerr;
			
			break;
	}
	free( path )
	if( exitcode ) return error( exitcode );
} // return 0 implicit, see C99 spec.
/* SISP */
