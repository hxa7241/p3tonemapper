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


#ifndef FpToInt_h
#define FpToInt_h




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/// fp to int ------------------------------------------------------------------


/**
 * Conversion from float to 32bit int, truncating fractional part.<br/><br/>
 *
 * Clamps to dword range.
 */
dword fpToIntTrunc( const float f );


/**
 * Conversion from float to 32bit int, rounding fractional part.<br/><br/>
 *
 * Clamps to dword range.<br/><br/>
 *
 * Some large magnitude fps may round inaccurately.
 */
inline
dword fpToIntRound
(
	const float f
)
{
	return fpToIntTrunc( f + (f >= 0.0f ? 0.5f : -0.5f) );
}


/**
 * Conversion from float to 32bit int, rounding fractional part.<br/><br/>
 *
 * fps outside dword range produce undefined results.<br/><br/>
 *
 * Some large magnitude fps may round inaccurately.
 */
inline
dword round
(
	const float f
)
{
	return dword( f + (f >= 0.0f ? 0.5f : -0.5f) );
}




/// fp to int 16.16 bit --------------------------------------------------------


/**
 * Conversion from float to 16.16bit fixed point int, truncating fractional
 * part.<br/><br/>
 *
 * Clamps to dword range.<br/><br/>
 */
inline
dword fpToInt1616
(
	const float f
)
{
	return fpToIntTrunc( f * 65536.0f );
}




/// fp [0-1) to int ------------------------------------------------------------


/**
 * Conversion from [0-1) float to 32bit int, truncating fractional part.
 * <br/><br/>
 *
 * fps outside range produce undefined results.<br/><br/>
 */
inline
dword fp01ToDword
(
	const float f
)
{
	return dword( udword( f * 4294967296.0f ) );
}


/**
 * Conversion from [0-1) float to 16bit int, truncating fractional part.
 * <br/><br/>
 *
 * fps outside range produce undefined results.<br/><br/>
 */
inline
dword fp01ToWord
(
	const float f
)
{
	return dword( f * 65536.0f );
}


/**
 * Conversion from [0-1) float to 8bit int, truncating fractional part.
 * <br/><br/>
 *
 * fps outside range produce undefined results.<br/><br/>
 */
inline
dword fp01ToByte
(
	const float f
)
{
	return dword( f * 256.0f );
}


/**
 * Conversion from [0-1) float to 4bit int, truncating fractional part.
 * <br/><br/>
 *
 * fps outside range produce undefined results.<br/><br/>
 */
inline
dword fp01ToNybble
(
	const float f
)
{
	return dword( f * 16.0f );
}




/// int to fp ------------------------------------------------------------------


struct FRACTIONALIZER
{
	static const float _16BIT;
	static const float  _8BIT;
	static const float  _4BIT;
};


/**
 * converts from 16bit int to float fraction.<br/><br/>
 *
 * repeats int bit pattern along fraction bits.
 */
inline
float uwordToFp
(
	const dword w
)
{
	return float((w << 8) | (w >> 8)) * FRACTIONALIZER::_16BIT;
}


/**
 * converts from 8bit int to float fraction.<br/><br/>
 *
 * repeats int bit pattern along fraction bits.
 */
inline
float ubyteToFp
(
	const dword b
)
{
	return float(b) * FRACTIONALIZER::_8BIT;
}


/**
 * converts from 4bit int to float fraction.<br/><br/>
 *
 * repeats int bit pattern along fraction bits.
 */
inline
float unybbleToFp
(
	const dword n
)
{
	return float(n) * FRACTIONALIZER::_4BIT;
}


}//namespace




#endif//FpToInt_h
