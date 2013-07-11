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


#include <math.h>

#include "Vector3f.hpp"

#include "ColorConstants.hpp"


using namespace hxa7241_graphics;




const float* hxa7241_graphics::ColorConstants::getSrgbChromaticities()
{
	// ITU-R BT.709 reference primaries (sRGB)
	static const float SRGB_CHROMATICITIES[] =
	{
		// x      y
		0.64f, 0.33f,   // r
		0.30f, 0.60f,   // g
		0.15f, 0.06f    // b
	};

	return SRGB_CHROMATICITIES;
}


const float* hxa7241_graphics::ColorConstants::getSrgbWhitePoint()
{
	// CIE standard illuminant D65 (ITU-R BT.709) (sRGB)
	static const float SRGB_WHITEPOINT[2] =
	{
		0.31273127312731f, 0.32903290329033f
	};

	return SRGB_WHITEPOINT;
}


float hxa7241_graphics::ColorConstants::getSrgbGamma()
{
	// sRGB standard gamma
	static const float SRGB_GAMMA = 1.0f / 2.2f;

	return SRGB_GAMMA;
}


float hxa7241_graphics::ColorConstants::get709Gamma()
{
	// ITU-R BT.709 standard gamma
	static const float _709_GAMMA = 0.45f;

	return _709_GAMMA;
}


float hxa7241_graphics::ColorConstants::gammaEncodeSrgb
(
	const float fp01
)
{
	float fp01prime = 0.0f;

	// sRGB standard transfer function
	if( fp01 <= 0.00304f )
	{
		fp01prime = 12.92f * fp01;
	}
	else
	{
		fp01prime = 1.055f * ::powf( fp01, 1.0f / 2.4f ) - 0.055f;
	}

	return fp01prime;
}


float hxa7241_graphics::ColorConstants::gammaEncode709
(
	const float fp01
)
{
	float fp01prime = 0.0f;

	// ITU-R BT.709 standard transfer function
	if( fp01 <= 0.018f )
	{
		fp01prime = 4.5f * fp01;
	}
	else
	{
		fp01prime = 1.099f * ::powf( fp01, 0.45f ) - 0.099f;
	}

	return fp01prime;
}


float hxa7241_graphics::ColorConstants::gammaDecodeSrgb
(
	const float fp01prime
)
{
	float fp01 = 0.0f;

	// sRGB standard inverse transfer function
	if( fp01prime <= 0.03928f )
	{
		fp01 = fp01prime * (1.0f / 12.92f);
	}
	else
	{
		fp01 = ::powf( (fp01prime + 0.055f) * (1.0f / 1.055f), 2.4f );
	}

	return fp01;
}


float hxa7241_graphics::ColorConstants::gammaDecode709
(
	const float fp01prime
)
{
	float fp01 = 0.0f;

	// ITU-R BT.709 standard inverse transfer function
	if( fp01prime <= 0.081f )
	{
		fp01 = fp01prime * (1.0f / 4.5f);
	}
	else
	{
		fp01 = ::powf( (fp01prime + 0.099f) * (1.0f / 1.099f), (1.0f / 0.45f) );
	}

	return fp01;
}


const float* hxa7241_graphics::ColorConstants::getCrtLuminanceRange()
{
	// approximate CRT luminance limits
	static const float CRT_BLACK =   2.0f;
	static const float CRT_WHITE = 100.0f;

	static const float CRT_RANGE[2] = { CRT_BLACK, CRT_WHITE };

	return CRT_RANGE;
}


const float* hxa7241_graphics::ColorConstants::getTftLuminanceRange()
{
	// approximate TFT luminance limits
	static const float TFT_BLACK =   2.0f;
	static const float TFT_WHITE = 150.0f;

	static const float TFT_RANGE[2] = { TFT_BLACK, TFT_WHITE };

	return TFT_RANGE;
}


const float* hxa7241_graphics::ColorConstants::getCrtBestLuminanceRange()
{
	// approximate CRT luminance limits
	static const float CRT_BLACK =   0.01f;
	static const float CRT_WHITE = 175.0f;

	static const float CRT_RANGE[2] = { CRT_BLACK, CRT_WHITE };

	return CRT_RANGE;
}


const float* hxa7241_graphics::ColorConstants::getTftBestLuminanceRange()
{
	// approximate TFT luminance limits
	// (backlight at medium level)
	static const float TFT_BLACK =   0.5f;
	static const float TFT_WHITE = 300.0f;

	static const float TFT_RANGE[2] = { TFT_BLACK, TFT_WHITE };

	return TFT_RANGE;
}


float hxa7241_graphics::ColorConstants::getLuminanceMin()
{
	static const float LUMINANCE_MIN = 1e-4f;

	return LUMINANCE_MIN;
}


float hxa7241_graphics::ColorConstants::getLuminanceMax()
{
	static const float LUMINANCE_MAX = 1e+6f;

	return LUMINANCE_MAX;
}
