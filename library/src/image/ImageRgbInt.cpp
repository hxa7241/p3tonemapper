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

#include "Clamps.hpp"
#include "FpToInt.hpp"
#include "ColorConstants.hpp"

#include "ImageRgbInt.hpp"   // own header is included last


using namespace p3tonemapper_image;








/// standard object services ---------------------------------------------------
ImageRgbInt::ImageRgbInt
(
	const dword width,
	const dword height,
	const bool  is48Bit,
	const bool  isAdopt,
	void*       pPixel3s
)
 :	width_m          ( hxa7241_general::clampMin( width,  0 ) )
 ,	height_m         ( hxa7241_general::clampMin( height, 0 ) )
 ,	bytesPerChannel_m( is48Bit ? 2 : 1  )
 ,	pPixel3s_m       ( pPixel3s )
 ,	isAdopt_m        ( isAdopt )
 ,	colorSpace_m     ()
 , gamma_m          ( 1.0f )
{
}


ImageRgbInt::~ImageRgbInt()
{
	if( isAdopt_m )
	{
		if( 2 == bytesPerChannel_m )
		{
			delete[] static_cast<uword*>( pPixel3s_m );
		}
		else
		{
			delete[] static_cast<ubyte*>( pPixel3s_m );
		}
	}
}


ImageRgbInt::ImageRgbInt
(
	const ImageRgbInt& other
)
{
	ImageRgbInt::operator=( other );
}


ImageRgbInt& ImageRgbInt::operator=
(
	const ImageRgbInt& other
)
{
	if( &other != this )
	{
		width_m           = other.width_m;
		height_m          = other.height_m;
		bytesPerChannel_m = other.bytesPerChannel_m;

		pPixel3s_m        = other.pPixel3s_m;
		isAdopt_m         = other.isAdopt_m;

		colorSpace_m      = other.colorSpace_m;
		gamma_m           = other.gamma_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void ImageRgbInt::setColorSpace
(
	const ColorSpace& colorSpace
)
{
	colorSpace_m = colorSpace;
}


void ImageRgbInt::setGamma
(
	const float gamma
)
{
	gamma_m = gamma;

	if( 0.0f != gamma_m )
	{
		gamma_m = hxa7241_general::clamp( gamma_m, 1e-2f, 1e+2f );
	}
}


void ImageRgbInt::setElement
(
	const dword  index,
	const float* pValue013
)
{
	if( 0 != pPixel3s_m )
	{
		using namespace hxa7241_general;

		for( dword i = 3;  i-- > 0; )
		{
			const dword indexTriple = index * 3;

			float channel01 = clamp01o( pValue013[i] );
			channel01       = 0.0f == gamma_m ?
				hxa7241_graphics::ColorConstants::gammaEncode709( channel01 ) :
				::powf( channel01, gamma_m );

			if( 2 == bytesPerChannel_m )
			{
				uword* pWord = static_cast<uword*>(pPixel3s_m) + indexTriple;
				pWord[i] = uword( fp01ToWord( channel01 ) );
			}
			else
			{
				ubyte* pByte = static_cast<ubyte*>(pPixel3s_m) + indexTriple;
				pByte[i] = ubyte( fp01ToByte( channel01 ) );
			}
		}
	}
}




/// queries --------------------------------------------------------------------
dword ImageRgbInt::getWidth() const
{
	return width_m;
}


dword ImageRgbInt::getHeight() const
{
	return height_m;
}


dword ImageRgbInt::getLength() const
{
	return height_m * width_m;
}


bool ImageRgbInt::is48Bit() const
{
	return 2 == getBytesPerChannel();
}


dword ImageRgbInt::getBytesPerChannel() const
{
	return bytesPerChannel_m;
}


const ColorSpace& ImageRgbInt::getColorSpace() const
{
	return colorSpace_m;
}


float ImageRgbInt::getGamma() const
{
	return gamma_m;
}


const void* ImageRgbInt::getPixels() const
{
	return pPixel3s_m;
}








/*/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>


namespace p3tonemapper_image
{
	using namespace hxa7241;


static std::ostream& operator<<
(
	std::ostream&   out,
	const Vector3w& v
)
{
	return out << "(" << v.getX() << "," << v.getY() << "," << v.getZ() << ")";
}


bool test_Vector3w
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_Vector3w ]\n\n";


	// basics
	{
		bool isOk_ = true;

		{
			const Vector3w v1;
			isOk_ &= (0 == v1.getX()) & (0 == v1.getY()) & (0 == v1.getZ());
			if( pOut && isVerbose ) *pOut << v1 << "  " << isOk_ << "\n";
		}
		{
			const Vector3w v2( 1, -2, 3000 );
			isOk_ &= (1 == v2.getX()) & (-2 == v2.getY()) & (3000 == v2.getZ());
			if( pOut && isVerbose ) *pOut << v2 << "  " << isOk_ << "\n";
		}
		{
			static const word w1[] = { 1, -2, 3000 };
			const Vector3w v3( w1 );
			isOk_ &= (1 == v3.getX()) & (-2 == v3.getY()) & (3000 == v3.getZ());
			if( pOut && isVerbose ) *pOut << v3 << "  " << isOk_ << "\n";
		}
		{
			static const float f1[] = { 0.0f, 0.25f, FLOAT_ALMOST_ONE };
			const Vector3w v4( f1 );
			isOk_ &= (word(hxa7241_general::fp01ToWord(f1[0])) == v4.getX()) &
				(word(hxa7241_general::fp01ToWord(f1[1])) == v4.getY()) &
				(word(hxa7241_general::fp01ToWord(f1[2])) == v4.getZ());
			if( pOut && isVerbose ) *pOut << v4 << "  " << word(hxa7241_general::fp01ToWord(f1[2])) << "  " << isOk_ << "\n";
		}
		{
			const Vector3w v5( 1, -2, 3000 );

			const Vector3w v6( v5 );
			isOk_ &= (v6.getX() == v5.getX()) &
				(v6.getY() == v5.getY()) & (v6.getZ() == v5.getZ());
			if( pOut && isVerbose ) *pOut << v6 << "  " << isOk_ << "\n";

			Vector3w v7;
			v7 = v5;
			isOk_ &= (v7.getX() == v5.getX()) &
				(v7.getY() == v5.getY()) & (v7.getZ() == v5.getZ());
			if( pOut && isVerbose ) *pOut << v7 << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "basics : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// commands
	{
		bool isOk_ = true;

		{
			Vector3w v2;
			v2.setXYZ( 1, -2, 3000 );
			isOk_ &= (1 == v2.getX()) & (-2 == v2.getY()) & (3000 == v2.getZ());
			if( pOut && isVerbose ) *pOut << v2 << "  " << isOk_ << "\n";
		}
		{
			static const word w1[] = { 1, -2, 3000 };
			Vector3w v3;
			v3.setXYZ( w1 );
			isOk_ &= (1 == v3.getX()) & (-2 == v3.getY()) & (3000 == v3.getZ());
			if( pOut && isVerbose ) *pOut << v3 << "  " << isOk_ << "\n";
		}
		{
			static const float f1[] = { 0.0f, 0.25f, FLOAT_ALMOST_ONE };
			Vector3w v4;
			v4.setXYZ( f1 );
			isOk_ &= (word(hxa7241_general::fp01ToWord(f1[0])) == v4.getX()) &
				(word(hxa7241_general::fp01ToWord(f1[1])) == v4.getY()) &
				(word(hxa7241_general::fp01ToWord(f1[2])) == v4.getZ());
			if( pOut && isVerbose ) *pOut << v4 << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "commands : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// queries
	{
		bool isOk_ = true;

		{
			const Vector3w v2( 1, -2, 3000 );
			isOk_ &= (1 == v2.getX()) & (-2 == v2.getY()) & (3000 == v2.getZ());
			if( pOut && isVerbose ) *pOut << v2 << "  " << isOk_ << "\n";
		}
		{
			const Vector3w v3( 1, -2, 3000 );
			word w1[3];
			v3.getXYZ( w1 );
			isOk_ &= (1 == w1[0]) & (-2 == w1[1]) & (3000 == w1[2]);
			if( pOut && isVerbose ) *pOut << v3 << "  " << isOk_ << "\n";
		}
		{
			const Vector3w v4( 1, -2, 3000 );
			const word* pw1;
			pw1 = v4.getXYZ();
			isOk_ &= (1 == pw1[0]) & (-2 == pw1[1]) & (3000 == pw1[2]);
			if( pOut && isVerbose ) *pOut << v4 << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "queries : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace


#endif//TESTING*/








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>


namespace p3tonemapper_image
{
	using namespace hxa7241;


bool test_ImageRgbInt
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_ImageRgbInt ]\n\n";


	// setElement positions
	{
		bool isFail = false;
		for( dword b = 1;  b <= 2;  ++b )
		{
			const dword width    = 4;
			const dword height   = 3;
			//const dword bits     = 8;
			ubyte       pixels[width * height * 3 * 2];
			void*       pPixel3s = pixels;

			ImageRgbInt image( width, height, 2 == b, false, pPixel3s );

			for( dword i = 0;  i < (width * height);  ++i )
			{
				const float major    = float(i) / 16.0f;
				const float minorDiv = (1 == b) ? 64.0f : 1024.0f;
				const float pixel[3] = {
					major + 0.0f/minorDiv, major + 1.0f/minorDiv, major + 2.0f/minorDiv };
				image.setElement( i, pixel );
			}

			for( dword i = 0;  i < (width * height);  ++i )
			{
				for( udword c = 0;  c < 3;  ++c )
				{
					udword value = 0;
					udword major = 0;
					udword minor = 0;
					if( 1 == b )
					{
						const ubyte* pPixel =
							static_cast<const ubyte*>(pPixel3s) + (i * 3);

						value = udword(pPixel[c]);

						major = udword(value) >> 4;
						minor = (udword(value) >> 2) & 3u;
					}
					else
					{
						const uword* pPixel =
							static_cast<const uword*>(pPixel3s) + (i * 3);

						value = udword(pPixel[c]);

						major = value >> 12;
						minor = ((udword(pPixel[c]) >> 6) & 3u);
					}

					isFail |= (udword(i) != major);
					isFail |= (c != minor);

					if( pOut && isVerbose ) *pOut << major << " " << minor << "   ";
				}
				if( pOut && isVerbose ) *pOut << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}

		if( pOut ) *pOut << "positions : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// setElement values
	{
		bool isFail = false;

		const float  value0[]  = { -1.0f, -2.0f, -3000.0f };
		const float  value1[]  = { 1.0f, 2.0f, 3000.0f };
		const float* pValues[] = { value0, value1 };

		for( dword v = 0;  v < 2;  ++v )
		{
			for( dword b = 1;  b <= 2;  ++b )
			{
				const dword width    = 4;
				const dword height   = 3;
				ubyte       pixels[width * height * 3 * 2];
				void*       pPixel3s = pixels;

				ImageRgbInt image( width, height, 2 == b, false, pPixel3s );

				image.setElement( 0, pValues[v] );

				for( udword c = 0;  c < 3;  ++c )
				{
					udword value = 0;
					if( 1 == b )
					{
						const ubyte* pPixel =
							static_cast<const ubyte*>(pPixel3s);

						value = udword(pPixel[c]);

						isFail |= (((0 == v) ? 0u : 0xFFu) != value);
					}
					else
					{
						const uword* pPixel =
							static_cast<const uword*>(pPixel3s);

						value = udword(pPixel[c]);

						isFail |= (((0 == v) ? 0u : 0xFFFFu) != value);
					}

					if( pOut && isVerbose ) *pOut << value << "   ";
				}
				if( pOut && isVerbose ) *pOut << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}

		if( pOut ) *pOut << "values : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace


#endif//TESTING
