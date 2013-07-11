/*--------------------------------------------------------------------

   Perceptuum3 rendering components
   Copyright (c) 2005, Harrison Ainsworth / HXA7241.

   http://www.hxa7241.org/

--------------------------------------------------------------------*/

/*--------------------------------------------------------------------

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later
   version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
   MA  02110-1301  USA

--------------------------------------------------------------------*/


#ifndef Clamps_h
#define Clamps_h




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/**
 * Clamp between min and max, open top interval.
 */
dword clampO( dword i, dword min, dword max );

/**
 * Clamp between min and max, closed interval.
 */
dword clamp( dword i, dword min, dword max );

/**
 * Clamp between min and max, closed interval.
 */
float clamp( float f, float min, float max );

/**
 * Clamp between 0 and 1, open top interval.
 */
float clamp01o( float f );

/**
 * Clamp between 0 and 1, closed interval.
 */
float clamp01c( float f );




/// INLINES ///

/**
 * Clamp between min and max, open top interval, inline.
 */
inline
dword clampO_
(
	      dword i,
	const dword min,
	const dword max
)
{
	if( i < min )
	{
		i = min;
	}
	else if( i >= max )
	{
		i = max - dword(1);
	}

	return i;
}


/**
 * Clamp between min and max, closed interval, inline.
 */
inline
dword clamp_
(
	      dword i,
	const dword min,
	const dword max
)
{
	if( i < min )
	{
		i = min;
	}
	else if( i > max )
	{
		i = max;
	}

	return i;
}


/**
 * Clamp between min and max, closed interval, inline.
 */
inline
float clamp_
(
	      float f,
	const float min,
	const float max
)
{
	if( f < min )
	{
		f = min;
	}
	else if( f > max )
	{
		f = max;
	}

	return f;
}


/**
 * Clamp between 0 and 1, open top interval, inline.
 */
inline
float clamp01o_
(
	float f
)
{
	if( f < 0.0f )
	{
		f = 0.0f;
	}
	else if( f >= 1.0f )
	{
		f = FLOAT_ALMOST_ONE;
	}

	return f;
}


/**
 * Clamp between 0 and 1, closed interval, inline.
 */
inline
float clamp01c_
(
	float f
)
{
	if( f < 0.0f )
	{
		f = 0.0f;
	}
	else if( f > 1.0f )
	{
		f = 1.0f;
	}

	return f;
}


/**
 * Clamp to min, closed half interval.
 */
inline
dword clampMin
(
	      dword i,
	const dword min
)
{
	if( i < min )
	{
		i = min;
	}

	return i;
}


/**
 * Clamp to max, closed half interval.
 */
inline
dword clampMax
(
	      dword i,
	const dword max
)
{
	if( i > max )
	{
		i = max;
	}

	return i;
}


/**
 * Clamp to min, closed half interval.
 */
inline
float clampMin
(
	      float f,
	const float min
)
{
	if( f < min )
	{
		f = min;
	}

	return f;
}


/**
 * Clamp to max, closed half interval.
 */
inline
float clampMax
(
	      float f,
	const float max
)
{
	if( f > max )
	{
		f = max;
	}

	return f;
}


}//namespace




#endif//Clamps_h
