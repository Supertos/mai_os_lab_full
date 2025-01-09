// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supertos Industries ( 2012 - 2024 )
// Author: Supertos
// Command line utilities
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "utils.h"

static char* ITOA_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char* revstr( char* s, size_t n ) {
	char temp;
	for( size_t i = 0; i < n / 2; i++ ) {
		temp = s[i];
		s[i] = s[n-i-1];
		s[n-i-1] = temp;
	}
	return s;
}

size_t strlen( const char* s ) {
	size_t out = 0;
	while( *(s++) != '\0' ) out++;
	
	return out;
}

void itoa( int n, char s[], uint8_t base ) {
	uint8_t i = 0;
	int sign = n < 0 ? -1 : 1;
 
    if( sign < 0 ) n = -n;
	
    do {      
        s[i++] = ITOA_ALPHABET[n % base]; 
    }while( (n /= base) > 0 );
    
    if( sign < 0 ) s[i++] = '-';
	
    s[i] = '\0';
    revstr(s, i);
}

int print( const char* text ) {
	return (int)write( stdout, text, strlen(text) );
}

int printn( const char* text, const size_t n ) {
	return (int)write( stdout, text, n );
}

int readn( char* text, const size_t n ) {
	return (int)read( stdin, text, n );
}

int error( const int code ) {	
	char* buf = malloc( ITOA_BUF_SIZE );
	itoa( code, buf, 10 );
	
	print( "An error has occured: " );
	print( buf );
	print( "\n" );
	
	free( buf );
	return code;
}

