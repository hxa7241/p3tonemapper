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


#ifndef ColorConstants_h
#define ColorConstants_h




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{


namespace ColorConstants
{
	/**
	 * ITU-R BT.709 reference primaries (sRGB).
	 *
	 * @return  pointer to a float[6] of: rx ry  gx gy  bx by
	 */
	const float* getSrgbChromaticities();

	/**
	 * CIE standard illuminant D65 (ITU-R BT.709) (sRGB).
	 *
	 * @return  pointer to a float[2]
	 */
	const float* getSrgbWhitePoint();

	/**
	 * sRGB standard gamma.
	 */
	float getSrgbGamma();

	/**
	 * ITU-R BT.709 standard gamma.
	 */
	float get709Gamma();

	/**
	 * sRGB standard gamma transfer function.
	 */
	float gammaEncodeSrgb( float fp01 );

	/**
	 * ITU-R BT.709 standard gamma transfer function.
	 */
	float gammaEncode709( float fp01 );

	/**
	 * sRGB standard gamma inverse transfer function.
	 */
	float gammaDecodeSrgb( float fp01prime );

	/**
	 * ITU-R BT.709 standard gamma inverse transfer function.
	 */
	float gammaDecode709( float fp01prime );


	/**
	 * Ordinary CRT range, in ordinary conditions -- overall, not very good.
	 *
	 * @return  pointer to array of 2 float: min and max
	 */
	const float* getCrtLuminanceRange();

	/**
	 * Ordinary TFT range, in ordinary conditions -- overall, not very good.
	 *
	 * @return  pointer to array of 2 float: min and max
	 */
	const float* getTftLuminanceRange();

	/**
	 * Good CRT range, in optimal conditions (2004).
	 *
	 * @return  pointer to array of 2 float: min and max
	 */
	const float* getCrtBestLuminanceRange();

	/**
	 * Good TFT range, in optimalconditions (2004).
	 *
	 * @return  pointer to array of 2 float: min and max
	 */
	const float* getTftBestLuminanceRange();

	/**
	 * Minimum luminance (human-visible).
	 */
	float getLuminanceMin();

	/**
	 * Maximum luminance (human-visible).
	 */
	float getLuminanceMax();
}


}//namespace




#endif//ColorConstants_h
