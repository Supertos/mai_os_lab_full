// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supertos Industries ( 2012 - 2024 )
// Author: Supertos
// Command line utilities
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#pragma once

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#define FLAG_MAX_LEN 5

typedef struct ProgramState ProgramState;
struct ProgramState;

typedef struct CMD CMD;
struct CMD {
	int argc;
	int argp;
	char** argv;
};

typedef struct FlagHandle FlagHandle;
struct FlagHandle {
	int (*handle)(CMD*, ProgramState*);
	char flag[FLAG_MAX_LEN];
};

typedef enum CMDType CMDType;
enum CMDType {
	Int = 0,
	Double = 1,
	String = 2,
	Flag = 3
};

//  Returns ProgramState struct. ProgramState definition is up to client code.
#define initProgramState() malloc( sizeof( ProgramState ) )

// Frees ProgramState struct back to OS
#define freeProgramState( state ) free( state )

// Returns CMD struct with fields initialized, where [argc] is parameter count and [argv] is parameter array.
CMD* initCMD( int argc, char** argv ); 

// Frees CMD struct back to OS
void freeCMD( CMD* command ); 

// Returns 1 if there's still parameters to be read in [command].
int isCMDEmpty( CMD* command ); 

// Returns pointer to current parameter in [command] and selects next one.
char* readCMD( CMD* command ); 

// Returns pointer to parameter of [expect] type in [command] and selects next parameter. Returns NULL on unexpected type.
char* expectCMD( CMD* command, CMDType expect );

// Executes flag handler from [handles] of [handlec] elements. Selects handler based on [flag] string and passes it [command].
int executeFlagHandler( CMD* command, const char* flag, FlagHandle handles[], size_t handlec, ProgramState* state ); 

// Goes through all of the given parameters in [command] and executes [handles] of [handlec] elements if corresponding flag found. Returns 0 on access, error code otherwise.
int processCMD( CMD* command, FlagHandle handles[], size_t handlec, ProgramState* state ); 