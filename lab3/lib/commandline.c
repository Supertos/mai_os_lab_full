// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supertos Industries ( 2012 - 2024 )
// Author: Supertos
// Command line utilities
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "commandline.h"
#include <stdio.h>

void freeCMD( CMD* command ) {
	free( command );
}

CMD* initCMD( int argc, char** argv ) {
	CMD* command = malloc( sizeof(CMD) );
	*command = (CMD){ .argc = argc, .argv = argv, .argp = 1 };
	
	return command;
}

int isCMDEmpty( CMD* command ) {
	return command->argc <= command->argp;
}

char* readCMD( CMD* command ) {
	if( isCMDEmpty( command ) ) return NULL;
	
	return command->argv[command->argp++];
}


char* expectCMD( CMD* command, CMDType expect ) {
	char* token = readCMD( command );
	if( !token ) return NULL;
	
	
	switch( expect ) {
		case Double:
		case Int:
			int flagDot = 0;
			for( size_t i = strlen( token ) - 1; i > 0; i-- ) { // CMP 0 is faster 
					if( ( token[i] < '0' || token[i] > '9' ) && token[i] != '-' && ( expect != Double || flagDot || token[i] != '.' ) ) return NULL;
					if( token[i] == '.' ) flagDot = 1;
				}
			break;
		case Flag:
			if( token[0] != '-' || strlen(token) < 2 ) return NULL;
			break;
		case String:
			break;
		default:
			return NULL;
	}
	return token;
}

int executeFlagHandler( CMD* command, const char* flag, FlagHandle handles[], size_t handlec, ProgramState* state ) {
	for( size_t i = 0; i < handlec; i++ ) {
		if( !strcmp( flag, handles[i].flag ) ) {
			return handles[i].handle(command, state);
		}
	}
	return 32; // No such flag err
}


int processCMD( CMD* command, FlagHandle handles[], size_t handlec, ProgramState* state ) {
	while( !isCMDEmpty( command ) ) {
		char* flag = expectCMD( command, Flag );
		if( !flag ) return 1;
		int code = executeFlagHandler( command, flag, handles, handlec, state );
		if( code ) return code;
	}
	return 0;
}