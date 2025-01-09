// -=-=-=-=-=-=-=-=-=-= Supertos Industries ( 2012 - 2024 ) =-=-=-=-=-=-=-=-=-=-
// Author: Supertos
// 
// Exercise 1
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include <math.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "commandline.h"
#include "utils.h"
#include "number.h"

/* Custom defines start here */
#define stdin 0
#define stdout 1

struct ProgramState { // Empty structures unallowed, see C99 spec.
	int dummy;
};

/* Helper function: checks if a is *a* prime */
int isPrime( double a ) {
	for( size_t i = 2; i <= sqrt(a); i++ )
		if( (size_t)a % i == 0 ) return 0;
	
	return 1;
}

/* Entry point */
int main( int argc, char** argv ) {
	char buffer[MAX_NUM_LEN + 1];
	while( read(stdin, buffer, MAX_NUM_LEN) ) {
		if( cstrtod( buffer ) < 0 || isPrime(cstrtod( buffer )) ) return NO_ERR;
		
		write( stdout, buffer, strlen(buffer) );
		write( stdout, "\n", 1 );
	}
	
} // return 0 implicit, see C99 spec.

/* SISP */