// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supertos Industries ( 2012 - 2024 )
// Author: Supertos
// Number utilities
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <math.h>
#include <string.h>

double cstrtod( const char* data ) {
	double whole = 0;
	double frac = 0;
	
	double sign = 1;
	if( data[0] == '-' ) {
		sign = -1;
		data++;
	}
	
	int exponent = 0; // 0 - before dot, >0 - after
	
	for( register char n = *data; n; n = *(++data) ) {
		if( n == '\0' ) break;
		
		if( n == '.' ) {
			if( exponent ) return 0;
			exponent = 1;
		}else if( n < '0' || n > '9' ) {
			return 0;
		}else if( exponent ) {
			frac += (n - '0') / pow(10, exponent++);
		}else{
			whole = whole*10 + (n - '0');
		}
	}
	return (whole + frac) * sign;
}

long double fact_simple( long double a ) {
	if( a == 0 ) return 1;
	
	for( long double n = a - 1; n > 0; n-- )
		a *= n;
	return a;
}

long double fact_simple_eveness( long double a ) {
	if( a == 0 ) return 1;
	
	long double init = a;
	for( long double n = a - 1; n > 0; n-- )
		if( (long int)init % 2 == (long int)a % 2 ) a *= n;
	return a;
}

long double fact_simple_trimmed( long double a, long double limit ) {
	if( a == 0 ) return 1;
	
	for( long double n = a - 1; n > limit; n-- )
		a *= n;
	return a;
}