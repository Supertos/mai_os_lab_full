// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supertos Industries ( 2012 - 2024 )
// Author: Supertos
// Number utilities
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/* Returns double extracted from [data] string */
double cstrtod( const char data[] );

/* Computes a! */
long double fact_simple( long double a );

/* Computes [a]!! */
long double fact_simple_eveness( long double a );

/* Computes [a]! / [limit]! !!![a] > [limit]!!! */
long double fact_simple_trimmed( long double a, long double limit );