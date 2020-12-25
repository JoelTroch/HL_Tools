#pragma once

#include <algorithm>
#include <cstdint>

#include "Platform.hpp"

/**
*	Utility functionality.
*/

/**
*	Initializes the random number generator.
*/
void UTIL_InitRandom();

/**
*	Gets a random 32 bit integer number in the range [iLow, iHigh]
*/
int UTIL_RandomLong( int iLow, int iHigh );

/**
*	Gets a random 32 bit floating point number in the range [flLow, flHigh]
*/
float UTIL_RandomFloat( float flLow, float flHigh );

/**
*	Returns a 1 bit at the given position.
*/
inline constexpr int32_t Bit( const size_t shift )
{
	return static_cast<int32_t>( 1 << shift );
}

/**
*	Returns a 1 bit at the given position.
*	64 bit variant.
*/
inline constexpr int64_t Bit64( const size_t shift )
{
	return static_cast<int64_t>( static_cast<int64_t>( 1 ) << shift );
}

/**
*	Sizeof for array types. Only works for arrays with a known size (stack buffers).
*	@tparam T Array type. Automatically inferred.
*	@tparam SIZE Number of elements in the array.
*	@return Number of elements in the array.
*/
template<typename T, const size_t SIZE>
constexpr inline size_t _ArraySizeof( T( &)[ SIZE ] )
{
	return SIZE;
}

/**
*	Replaces ARRAYSIZE. ARRAYSIZE is defined in some platform specific headers.
*/
#define ARRAYSIZE _ArraySizeof

//TODO: this might be better off in a static library;

/**
*	Returns the current tick time, in milliseconds.
*	@return Tick time, in milliseconds.
*/
long long GetCurrentTick();

/**
*	Gets the current time, in seconds.
*	@return Current time, in seconds.
*/
double GetCurrentTime();

template<typename TFlags, typename TFlag>
TFlags SetFlags(TFlags flags, TFlag flag, bool set = true)
{
	const TFlags convertedValue{static_cast<TFlags>(flag)};

	if (set)
	{
		return flags | convertedValue;
	}
	else
	{
		return flags & ~convertedValue;
	}
}
