#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#include <cstring>

/**
*	Utility functionality.
*/

/**
*	Checks if a printf operation was successful
*/
inline bool PrintfSuccess( const int iRet, const size_t uiBufferSize )
{
	return iRet >= 0 && static_cast<size_t>( iRet ) < uiBufferSize;
}

/**
*	Taken from MSVC string hash.
*/
inline size_t StringHash( const char* const pszString )
{
#if defined(_WIN64)
	static_assert( sizeof( size_t ) == 8, "This code is for 64-bit size_t." );
	const size_t _FNV_offset_basis = 14695981039346656037ULL;
	const size_t _FNV_prime = 1099511628211ULL;

#else /* defined(_WIN64) */
	static_assert( sizeof( size_t ) == 4, "This code is for 32-bit size_t." );
	const size_t _FNV_offset_basis = 2166136261U;
	const size_t _FNV_prime = 16777619U;
#endif /* defined(_WIN64) */

	const size_t _Count = strlen( pszString );

	size_t _Val = _FNV_offset_basis;
	for( size_t _Next = 0; _Next < _Count; ++_Next )
	{	// fold in another byte
		_Val ^= ( size_t ) pszString[ _Next ];
		_Val *= _FNV_prime;
	}
	return ( _Val );
}

/**
*	Gets a random 32 bit integer number in the range [iLow, iHigh]
*/
int UTIL_RandomLong( int iLow, int iHigh );

/**
*	Gets a random 32 bit floating point number in the range [flLow, flHigh]
*/
float UTIL_RandomFloat( float flLow, float flHigh );

#endif //COMMON_UTILITY_H