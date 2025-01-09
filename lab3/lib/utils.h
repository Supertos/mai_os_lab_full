// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supertos Industries ( 2012 - 2024 )
// Author: Supertos
// Command line utilities
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <stdint.h>
#include <stdlib.h>

#define stdin 0
#define stdout 1

#define IS_BLANK(c) c == ' ' || c == '\t' || c == '\r' || c == '\n'
#define IS_DIGIT(c) c >= '0' && c <= '9'

#define ITOA_BUF_SIZE 32
#define MAX_PATH_LEN 512
#define MAX_NUM_LEN 512

#define NO_ERR 0
#define ERR_PATH_READ 1
#define ERR_INVALID_INPUT 2
#define ERR_CANT_INIT_PIPE 4
#define ERR_CANT_INIT_CHILD 8
#define ERR_NO_SUCH_FILE 16

/* Returns length of [s] */
size_t strlen( const char* s );

/* Reverses string [s] of size [n] */
char* revstr( char* s, size_t n );

/* Dumps [n] to [s] in [base] notation */
void itoa(int n, char s[], uint8_t base);

/* Prints null-terminated [text] to stdout (1) */
int print( const char* text );

/* Prints [n] chars from [text] to stdout (1) */
int printn( const char* text, const size_t n  );

/* Read [n] chars from stdin (0) to [text] */
int readn( char* text, const size_t n );

/* Prints custom error [text] and returns [code] */
int error( const int code );