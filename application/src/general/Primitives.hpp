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


#ifndef Primitives_h
#define Primitives_h


#include <limits.h>
//#include <limits>

#include <float.h>




namespace hxa7241
{

/// types ----------------------------------------------------------------------

	typedef  signed   char   byte;
	typedef  unsigned char   ubyte;

	typedef  signed   short  word;
	typedef  unsigned short  uword;

	typedef  signed   int    dword;
	typedef  unsigned int    udword;

	typedef  float           fp;


/// constants ------------------------------------------------------------------
/// limits.h version

	const byte   BYTE_MIN    = SCHAR_MIN;
	const byte   BYTE_MAX    = SCHAR_MAX;
	const int    BYTE_BITS   = 8;

	const ubyte  UBYTE_MIN   = 0;
	const ubyte  UBYTE_MAX   = UCHAR_MAX;
	const int    UBYTE_BITS  = 8;


	const word   WORD_MIN    = SHRT_MIN;
	const word   WORD_MAX    = SHRT_MAX;
	const int    WORD_BITS   = 16;

	const uword  UWORD_MIN   = 0;
	const uword  UWORD_MAX   = USHRT_MAX;
	const int    UWORD_BITS  = 16;


	const dword  DWORD_MIN   = INT_MIN;
	const dword  DWORD_MAX   = INT_MAX;
	const int    DWORD_BITS  = 32;

	const udword UDWORD_MIN  = 0;
	const udword UDWORD_MAX  = UINT_MAX;
	const int    UDWORD_BITS = 32;


	const float  FLOAT_MIN_POS     = float(FLT_MIN);
	const float  FLOAT_MIN_NEG     = float(-FLT_MAX);
	const float  FLOAT_MAX         = float(FLT_MAX);
	const float  FLOAT_EPSILON     = float(FLT_EPSILON);
	const float  FLOAT_ALMOST_ONE  = float(1.0f - FLT_EPSILON);
	const float  FLOAT_SMALL       = float(1.0e-12f);
	const float  FLOAT_LARGE       = float(1.0e+12f);

	const double DOUBLE_MIN_POS    = double(DBL_MIN);
	const double DOUBLE_MIN_NEG    = double(-DBL_MAX);
	const double DOUBLE_MAX        = double(DBL_MAX);
	const double DOUBLE_EPSILON    = double(DBL_EPSILON);
	const double DOUBLE_ALMOST_ONE = double(1.0 - DBL_EPSILON);
	const double DOUBLE_SMALL      = double(1.0e-96);
	const double DOUBLE_LARGE      = double(1.0e+96);

	// must match fp typedef, above
	const float  FP_MIN_POS        = FLOAT_MIN_POS;
	const float  FP_MIN_NEG        = FLOAT_MIN_NEG;
	const float  FP_MAX            = FLOAT_MAX;
	const float  FP_EPSILON        = FLOAT_EPSILON;
	const float  FP_ALMOST_ONE     = FLOAT_ALMOST_ONE;
	const float  FP_SMALL          = FLOAT_SMALL;
	const float  FP_LARGE          = FLOAT_LARGE;


/// limits version
/*	const byte   BYTE_MIN    = std::numeric_limits<byte>::min();
	const byte   BYTE_MAX    = std::numeric_limits<byte>::max();
	const int    BYTE_BITS   = 8;

	const ubyte  UBYTE_MIN   = std::numeric_limits<ubyte>::min();
	const ubyte  UBYTE_MAX   = std::numeric_limits<ubyte>::max();
	const int    UBYTE_BITS  = 8;


	const word   WORD_MIN    = std::numeric_limits<word>::min();
	const word   WORD_MAX    = std::numeric_limits<word>::max();
	const int    WORD_BITS   = 16;

	const uword  UWORD_MIN   = std::numeric_limits<uword>::min();
	const uword  UWORD_MAX   = std::numeric_limits<uword>::max();
	const int    UWORD_BITS  = 16;


	const dword  DWORD_MIN   = std::numeric_limits<dword>::min();
	const dword  DWORD_MAX   = std::numeric_limits<dword>::max();
	const int    DWORD_BITS  = 32;

	const udword UDWORD_MIN  = std::numeric_limits<udword>::min();
	const udword UDWORD_MAX  = std::numeric_limits<udword>::max();
	const int    UDWORD_BITS = 32;


	const float  FLOAT_MIN_POS     = std::numeric_limits<float>::min();
	const float  FLOAT_MIN_NEG     = float(-std::numeric_limits<float>::max());
	const float  FLOAT_MAX         = std::numeric_limits<float>::max();
	const float  FLOAT_EPSILON     = std::numeric_limits<float>::epsilon();
	const float  FLOAT_ALMOST_ONE  = float(1.0f - std::numeric_limits<float>::epsilon());
	const float  FLOAT_SMALL       = float(1.0e-12f);
	const float  FLOAT_LARGE       = float(1.0e+12f);

	const double DOUBLE_MIN_POS    = std::numeric_limits<double>::min();
	const double DOUBLE_MIN_NEG    = double(-std::numeric_limits<double>::max());
	const double DOUBLE_MAX        = std::numeric_limits<double>::max();
	const double DOUBLE_EPSILON    = std::numeric_limits<double>::epsilon();
	const double DOUBLE_ALMOST_ONE = double(1.0 - std::numeric_limits<double>::epsilon());
	const double DOUBLE_SMALL      = double(1.0e-96);
	const double DOUBLE_LARGE      = double(1.0e+96);

	// must match fp typedef, above
	const float  FP_MIN_POS        = FLOAT_MIN_POS;
	const float  FP_MIN_NEG        = FLOAT_MIN_NEG;
	const float  FP_MAX            = FLOAT_MAX;
	const float  FP_EPSILON        = FLOAT_EPSILON;
	const float  FP_ALMOST_ONE     = FLOAT_ALMOST_ONE;
	const float  FP_SMALL          = FLOAT_SMALL;
	const float  FP_LARGE          = FLOAT_LARGE;*/

}//namespace




#endif//Primitives_h
