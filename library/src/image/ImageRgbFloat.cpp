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
#include "Vector3f.hpp"
#include "ImageRgbFloatIter.hpp"

#include "ImageRgbFloat.hpp"   // own header is included last


using namespace p3tonemapper_image;




/// standard object services ---------------------------------------------------
ImageRgbFloat::ImageRgbFloat()
 :	sheet_m     ()
 ,	colorSpace_m()
{
}


ImageRgbFloat::ImageRgbFloat
(
	const dword width,
	const dword height
)
 :	sheet_m     ()
 ,	colorSpace_m()
{
	ImageRgbFloat::setImage( width, height );
}


ImageRgbFloat::ImageRgbFloat
(
	const dword       width,
	const dword       height,
	float*const       pInPixel3s,
	const bool        isAdopt,
	const ColorSpace& colorSpace
)
 :	sheet_m     ()
 ,	colorSpace_m()
{
	ImageRgbFloat::setImage( width, height, pInPixel3s, isAdopt, colorSpace );
}


ImageRgbFloat::~ImageRgbFloat()
{
}


ImageRgbFloat::ImageRgbFloat
(
	const ImageRgbFloat& other
)
 :	sheet_m     ()
 ,	colorSpace_m()
{
	ImageRgbFloat::assign( other );
}


ImageRgbFloat& ImageRgbFloat::operator=
(
	const ImageRgbFloat& other
)
{
	assign( other );

	return *this;
}




/// commands -------------------------------------------------------------------
void ImageRgbFloat::setImage
(
	const dword width,
	const dword height
)
{
	sheet_m.setSize( width * 3, height );

	zeroValues();
}


void ImageRgbFloat::setImage
(
	const dword       width,
	const dword       height,
	float*const       pInPixel3s,
	const bool        isAdopt,
	const ColorSpace& colorSpace
)
{
	sheet_m.setMemory( pInPixel3s, width * 3, height, isAdopt );
	setColorSpace( colorSpace );

	clampValues();
}


void ImageRgbFloat::setColorSpace
(
	const ColorSpace& colorSpace
)
{
	colorSpace_m = colorSpace;
}


void ImageRgbFloat::set
(
	const dword     x,
	const dword     y,
	const Vector3f& element
)
{
	float* pF = &( sheet_m.get( x * 3, y ) );

	Vector3f elementClamped( element );
	elementClamped.clampBetween( Vector3f::ZERO(), Vector3f::LARGE() );

	elementClamped.getXYZ( pF );
}


void ImageRgbFloat::set
(
	const dword     i,
	const Vector3f& element
)
{
	float* pF = &( sheet_m[ i * 3 ] );

	Vector3f elementClamped( element );
	elementClamped.clampBetween( Vector3f::ZERO(), Vector3f::LARGE() );

	elementClamped.getXYZ( pF );
}


ImageRgbFloatIter ImageRgbFloat::getIterator()
{
	return ImageRgbFloatIter( sheet_m.getMemory() );
}


void ImageRgbFloat::zeroValues()
{
	sheet_m.zeroMemory();
}


void ImageRgbFloat::clampValues()
{
	for( dword i = getLength();  i-- > 0; )
	{
		sheet_m[i] = hxa7241_general::clamp_( sheet_m[i], 0.0f, FLOAT_LARGE );
	}
}




/// queries --------------------------------------------------------------------
dword ImageRgbFloat::getLength() const
{
	return sheet_m.getLength() / 3;
}


dword ImageRgbFloat::getWidth() const
{
	return sheet_m.getWidth() / 3;
}


dword ImageRgbFloat::getHeight() const
{
	return sheet_m.getHeight();
}


bool ImageRgbFloat::isOwning() const
{
	return sheet_m.isOwning();
}


const ColorSpace& ImageRgbFloat::getColorSpace() const
{
	return colorSpace_m;
}


void ImageRgbFloat::get
(
	const dword x,
	const dword y,
	Vector3f&   element
) const
{
	const float* pF = &( sheet_m.get( x * 3, y ) );

	element.setXYZ( pF );
}


Vector3f ImageRgbFloat::get
(
	const dword x,
	const dword y
) const
{
	const float* pF = &( sheet_m.get( x * 3, y ) );

	return Vector3f( pF );
}


void ImageRgbFloat::get
(
	const dword i,
	Vector3f&   element
) const
{
	const float* pF = &( sheet_m[ i * 3 ] );

	element.setXYZ( pF );
}


Vector3f ImageRgbFloat::get
(
	const dword i
) const
{
	const float* pF = &( sheet_m[ i * 3 ] );

	return Vector3f( pF );
}


ImageRgbFloatIterConst ImageRgbFloat::getIteratorConst() const
{
	return ImageRgbFloatIterConst( sheet_m.getMemory() );
}




/// statics --------------------------------------------------------------------
bool ImageRgbFloat::isSizeWithinRange
(
	const dword width,
	const dword height
)
{
	return Sheet<float>::isSizeWithinRange( width * 3, height );
}


dword ImageRgbFloat::getMaxSize()
{
	return Sheet<float>::getMaxSize() / 3;
}


void ImageRgbFloat::visitBilinear
(
	const ImageRgbFloat& in,
	BilinearVisitor&     visitor,
	const dword          outWidth,
	const dword          outHeight
)
{
	const dword inWidth  = in.getWidth();
	const dword inHeight = in.getHeight();

	// output larger than input
	if( (inWidth  <= outWidth ) &&
	    (inHeight <= outHeight) )
	{
		// simple bilinear interpolation, optimise later if needed...

		const float xScale = float(inWidth)  / float(outWidth);
		const float yScale = float(inHeight) / float(outHeight);

		// loop through output pixels
		for( dword oy = 0;  oy < outHeight;  ++oy )
		{
			for( dword ox = 0;  ox < outWidth;  ++ox )
			{
				// calc fp coords in input grid
				// (pixel samples are at the centers of their pixel squares)
				const float ixFp = (float(ox) + 0.5f) * xScale - 0.5f;
				const float iyFp = (float(oy) + 0.5f) * yScale - 0.5f;

				// calc four surrounding int coords, clamped inside image
				dword ixLo   = dword( ::floorf(ixFp) );
				dword ixHi   = ixLo + 1;
				float ixFrac = ixFp - float(ixLo);
				ixLo = ixLo <  0       ? 0             : ixLo;
				ixHi = ixHi >= inWidth ? (inWidth - 1) : ixHi;

				dword iyLo   = dword( ::floorf(iyFp) );
				dword iyHi   = iyLo + 1;
				float iyFrac = iyFp - float(iyLo);
				iyLo = iyLo <  0        ? 0              : iyLo;
				iyHi = iyHi >= inHeight ? (inHeight - 1) : iyHi;

				// get surrounding input values
				const Vector3f i00( in.get( ixLo, iyLo ) );
				const Vector3f i01( in.get( ixLo, iyHi ) );
				const Vector3f i10( in.get( ixHi, iyLo ) );
				const Vector3f i11( in.get( ixHi, iyHi ) );

				// interpolate values bilinearly
				// (more accurate, but might under/overflow)
				const Vector3f left ( i00 + ((i01 - i00) * iyFrac) );
				const Vector3f right( i10 + ((i11 - i10) * iyFrac) );
				const Vector3f inPixel( left + ((right - left) * ixFrac) );
				// (less accurate, but wont under/overflow)
				//const Vector3f left ( (i01 * iyFrac) + (i00 * (FLOAT_ALMOST_ONE - iyFrac)) );
				//const Vector3f right( (i11 * iyFrac) + (i10 * (FLOAT_ALMOST_ONE - iyFrac)) );
				//const Vector3f inPixel((right * ixFrac) + (left * (FLOAT_ALMOST_ONE - ixFrac)));

				// operate on interpolated input pixel
				visitor.operate( inPixel, ox, oy );
			}
		}
	}
	// output smaller than input
	else
	{
		// not implemented until needed...
		throw "ImageRgbFloat:visitBilinear - smaller output not implemented";
	}
}




/// implementation -------------------------------------------------------------
void ImageRgbFloat::assign
(
	const ImageRgbFloat& other
)
{
	if( &other != this )
	{
		sheet_m      = other.sheet_m;
		colorSpace_m = other.colorSpace_m;
	}
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>


namespace p3tonemapper_image
{
	using namespace hxa7241;


static dword random_m = 0;

static void setRand( const dword seed )
{
	random_m = seed;
}

static dword getRand()
{
	random_m = dword(1664525) * random_m + dword(1013904223);
	return random_m;
}

static float getRandFp()
{
	return float(getRand()) / 2147483648.0f;
}


static bool roughlyEqual
(
	const float a,
	const float b
)
{
	return 1e-3f > ::fabsf( a - b );
}


bool test_ImageRgbFloat
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_ImageRgbFloat ]\n\n";

	setRand( seed );


	// constructor and queries
	{
		bool isOk_ = true;

		float p1[] = {
			0x000, 0x001, 0x002, 0x010, 0x011, 0x012, 0x020, 0x021, 0x022,
			0x100, 0x101, 0x102, 0x110, 0x111, 0x112, 0x120, 0x121, 0x122
		};
		const dword width  = 3;
		const dword height = 2;
		const ColorSpace c1;
		ImageRgbFloat i1( width, height, &(p1[0]), false, c1 );

		{
			isOk_ &= (width * height) == i1.getLength();
			isOk_ &= width == i1.getWidth();
			isOk_ &= height == i1.getHeight();
			isOk_ &= !i1.isOwning();
			if( pOut && isVerbose ) *pOut << "length: " << i1.getLength() <<
				"  width: " << i1.getWidth() << "  height: " << i1.getHeight() <<
				"  owning: " << i1.isOwning() << "\n";
		}

		for( dword y = 0;  y < height;  ++y )
		{
			for( dword x = 0;  x < width;  ++x )
			{
				const dword xy = (x + (y * width)) * 3;

				const Vector3f ip( i1.get( x, y ) );
				float ipa[3];
				ip.getXYZ( ipa );

				for( dword i = 0;  i < 3;  ++i )
				{
					isOk_ &= p1[xy + i] == ipa[i];

					isOk_ &= (i == (dword(ipa[i]) & 0x000F));
					isOk_ &= (x == ((dword(ipa[i]) & 0x00F0) >> 4));
					isOk_ &= (y == ((dword(ipa[i]) & 0x0F00) >> 8));
				}

				{
					Vector3f ip2;
					i1.get( x, y, ip2 );
					isOk_ &= (ip == ip2);

					Vector3f ip3;
					i1.get( xy / 3, ip3 );
					isOk_ &= (Vector3f(p1 + xy) == ip3);
					isOk_ &= (Vector3f(p1 + xy) == i1.get( xy / 3 ));
				}

				if( pOut && isVerbose ) *pOut << "[" << ip.getX() << " " <<
					ip.getY() << " " << ip.getZ() << "] " << isOk_ << " ";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "constructor and queries : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// commands
	{
		bool isOk_ = true;

		// set
		{
			float p1[] = {
				0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			const dword wi = 3;
			const dword he = 2;
			const ColorSpace c1;
			ImageRgbFloat i1( wi, he, p1, false, c1 );

			for( dword y = 0;  y < he;  ++y )
			{
				for( dword x = 0;  x < wi;  ++x )
				{
					const dword b = (y << 8) + (x << 4);
					i1.set( x, y, Vector3f(
						float(b + 0), float(b + 1), float(b + 2) ) );
				}
			}

			for( dword y = 0;  y < he;  ++y )
			{
				for( dword x = 0;  x < wi;  ++x )
				{
					const Vector3f ip( i1.get( x, y ) );
					float ipa[3];
					ip.getXYZ( ipa );

					for( dword i = 0;  i < 3;  ++i )
					{
						isOk_ &= (i == (dword(ipa[i]) & 0x000F));
						isOk_ &= (x == ((dword(ipa[i]) & 0x00F0) >> 4));
						isOk_ &= (y == ((dword(ipa[i]) & 0x0F00) >> 8));
					}

					if( pOut && isVerbose ) *pOut << "(" << ip.getX() << " " <<
						ip.getY() << " " << ip.getZ() << ") " << isOk_ << " ";
				}
				if( pOut && isVerbose ) *pOut << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}

		// zeroValues
		{
			float p1[] = {
				1, 2, 3,  4, 5, 6,  7, 8, 9,
				10, 11, 12,  13, 14, 15,  16, 17, 18
			};
			const dword wi = 3;
			const dword he = 2;
			const ColorSpace c1;
			ImageRgbFloat i1( wi, he, p1, false, c1 );

			i1.zeroValues();

			for( dword i = i1.getLength();  i-- > 0; )
			{
				const Vector3f ip( i1.get( i ) );
				isOk_ &= (Vector3f::ZERO() == ip);
				if( pOut && isVerbose ) *pOut << "(" << ip.getX() << " " <<
					ip.getY() << " " << ip.getZ() << ") " << isOk_ << " ";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "commands : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// iterators
	{
		bool ok = true;

		float a1[] = {
			1, 2, 3,  4, 5, 6,  7, 8, 9,
			10, 11, 12,  13, 14, 15,  16, 17, 18
		};
		const dword wi = 3;
		const dword he = 2;
		const ColorSpace c1;
		ImageRgbFloat i1( wi, he, a1, false, c1 );
		ImageRgbFloatIter iter( i1.getIterator() );

		// copy
		const ImageRgbFloatIter iter2( iter );
		ok &= iter2 == iter;
		if( pOut && isVerbose ) *pOut << "copy  " << ok << "\n";

		// inc and dec
		{
			++iter;
			ok &= iter2 < iter;
			--iter;
			ok &= iter2 == iter;

			if( pOut && isVerbose ) *pOut << "inc dec  " << ok << "\n";
		}

		// add and sub
		{
			iter = iter + 3;
			ok &= iter2 < iter;
			iter = iter - 3;
			ok &= iter2 == iter;

			if( pOut && isVerbose ) *pOut << "add sub  " << ok << "\n";
		}

		// add and sub eq
		{
			iter += 3;
			ok &= iter2 < iter;
			iter -= 3;
			ok &= iter2 == iter;

			if( pOut && isVerbose ) *pOut << "add sub eq  " << ok << "\n";
		}

		// inc and add
		{
			ImageRgbFloatIter iter3( iter );

			++iter;
			iter3 += 1;
			ok &= iter3 == iter;

			--iter;
			iter3 -= 1;
			ok &= iter3 == iter;

			if( pOut && isVerbose ) *pOut << "inc and add  " << ok << "\n";
		}

		// comparisons
		{
			ok &= iter == iter2;
			ok &= iter2 == iter;

			ok &= iter >= iter2;
			ok &= iter2 <= iter;

			++iter;

			ok &= iter != iter2;
			ok &= iter2 != iter;

			ok &= iter > iter2;
			ok &= iter2 < iter;
			ok &= iter >= iter2;
			ok &= iter2 <= iter;

			if( pOut && isVerbose ) *pOut << "comparisons  " << ok << "\n";
		}

		// access
		{
			iter = iter2;
			++iter;

			// get
			Vector3f v1;
			iter.get( v1 );
			Vector3f v2( iter.get() );
			ok &= v1 == v2;
			ok &= v1 == Vector3f( a1 + 3 );

			// set
			Vector3f v3( 111.0f, 222.0f, 333.0f );
			iter.set( v3 );
			ok &= v3 == Vector3f( a1 + 3 );

			if( pOut && isVerbose ) *pOut << "access  " << ok << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "iterators : " <<
			(ok ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= ok;
	}


	// visitBilinear
	{
		struct TestBilinearVisitor
			: public ImageRgbFloat::BilinearVisitor
		{
			ImageRgbFloat* pi2_m;

			TestBilinearVisitor( ImageRgbFloat& i2 )
			 :	pi2_m( &i2 )
			{
			}

			void  operate( const Vector3f& interpolatedPixel,
								dword           outX,
								dword           outY )
			{
				pi2_m->set( outX, outY, interpolatedPixel );
			}
		};


		// flat field -> flat field (channels stay separate)
		{
			bool isOk_ = true;

			const Vector3f p1( 42.125f, 0.0f, 1001.0f );

			ImageRgbFloat i1( 4, 4 );
			for( dword i = i1.getLength();  i-- > 0; )
			{
				i1.set( i, p1 );
			}

			ImageRgbFloat i2( 11, 11 );
			TestBilinearVisitor v1( i2 );

			ImageRgbFloat::visitBilinear( i1, v1, i2.getWidth(), i2.getHeight() );

			for( dword i = i2.getLength();  i-- > 0; )
			{
				const Vector3f dif( (p1 - i2.get(i)).abs() );
				// bilinear interpolation should be exact in this case
				isOk_ &= dif.largest() == 0.0f; //< 1e-5f;

				//if( pOut && isVerbose ) *pOut << i2.get(i) << " ";
			}

			//if( pOut && isVerbose ) *pOut << "\n\n";

			if( pOut ) *pOut << "flat : " <<
				(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

			isOk &= isOk_;
		}


		// ramp (vertical and horizontal) -> ramp
		{
			bool isOk_ = true;

			ImageRgbFloat i1( 4, 4 );
			for( dword y = i1.getHeight();  y-- > 0; )
			{
				for( dword x = i1.getWidth();  x-- > 0; )
				{
					// x channel is horizontal ramp
					// y channel is vertical ramp
					const Vector3f v(
						float(x) / float(i1.getWidth()),
						float(y) / float(i1.getHeight()),
						42.125f );
					i1.set( x, y, v );

					if( pOut && isVerbose ) *pOut << v << " ";
				}
				if( pOut && isVerbose ) *pOut << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";

			ImageRgbFloat i2( 11, 11 );
			TestBilinearVisitor v1( i2 );

			ImageRgbFloat::visitBilinear( i1, v1, i2.getWidth(), i2.getHeight() );

			for( dword y = 0;  y < i2.getHeight();  ++y )
			{
				Vector3f last( i2.get( 0, y ) );
				for( dword x = 1;  x < i2.getWidth();  ++x )
				{
					const Vector3f current( i2.get( x, y ) );
					isOk_ &= current.getX() >  last.getX();
					isOk_ &= current.getY() == last.getY();
					isOk_ &= current.getZ() == 42.125f;

					if( pOut && isVerbose ) *pOut << last.getX() << " ";

					last = current;
				}
				if( pOut && isVerbose ) *pOut << last.getX() << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";

			for( dword x = 0;  x < i2.getWidth();  ++x )
			{
				Vector3f last( i2.get( x, 0 ) );
				for( dword y = 1;  y < i2.getHeight();  ++y )
				{
					const Vector3f current( i2.get( x, y ) );
					isOk_ &= current.getX() == last.getX();
					isOk_ &= current.getY() >  last.getY();
					isOk_ &= current.getZ() == 42.125f;

					if( pOut && isVerbose ) *pOut << last.getY() << " ";

					last = current;
				}
				if( pOut && isVerbose ) *pOut << last.getY() << "\n";
			}

			if( pOut && isVerbose ) *pOut << "\n";

			if( pOut ) *pOut << "ramps1 : " <<
				(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

			isOk &= isOk_;
		}


		// line -> symmetrical ramp
		{
			bool isOk_ = true;

			ImageRgbFloat i1( 5, 5 );
			for( dword y = i1.getHeight();  y-- > 0; )
			{
				for( dword x = i1.getWidth();  x-- > 0; )
				{
					// x channel is horizontal mid line
					// z channel is vertical mid line
					const Vector3f v(
						y == (i1.getHeight() / 2) ? 1024.0f : 0.0f,
						42.125f,
						x == (i1.getWidth()  / 2) ?  128.0f : 0.0f );
					i1.set( x, y, v );

					if( pOut && isVerbose ) *pOut << "(" << v.getX() << " "<< v.getZ() << ") ";
				}
				if( pOut && isVerbose ) *pOut << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";

			ImageRgbFloat i2( 11, 11 );
			TestBilinearVisitor v1( i2 );

			ImageRgbFloat::visitBilinear( i1, v1, i2.getWidth(), i2.getHeight() );

			for( dword y = 0;  y < i2.getHeight();  ++y )
			{
				Vector3f last( i2.get( 0, y ) );
				for( dword x = 1;  x < i2.getWidth();  ++x )
				{
					const Vector3f current( i2.get( x, y ) );
					isOk_ &= current.getX() == last.getX();
					isOk_ &= current.getY() == 42.125f;
					isOk_ &= roughlyEqual( current.getZ(),
						i2.get( i2.getWidth() - x - 1, y ).getZ() );

					if( pOut && isVerbose ) *pOut << last.getX() << " ";

					last = current;
				}
				if( pOut && isVerbose ) *pOut << last.getX() << "\n";
			}
			if( pOut && isVerbose ) *pOut << "\n";

			for( dword x = 0;  x < i2.getWidth();  ++x )
			{
				Vector3f last( i2.get( x, 0 ) );
				for( dword y = 1;  y < i2.getHeight();  ++y )
				{
					const Vector3f current( i2.get( x, y ) );
					isOk_ &= roughlyEqual( current.getX(),
						i2.get( x, i2.getHeight() - y - 1 ).getX() );
					isOk_ &= current.getY() == 42.125f;
					isOk_ &= current.getZ() == last.getZ();

					if( pOut && isVerbose ) *pOut << last.getZ() << " ";

					last = current;
				}
				if( pOut && isVerbose ) *pOut << last.getZ() << "\n";
			}

			if( pOut && isVerbose ) *pOut << "\n";

			if( pOut ) *pOut << "ramps2 : " <<
				(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

			isOk &= isOk_;
		}


		// resultant pixel channels within bounds of input min-maxs
		{
			bool isOk_ = true;

			Vector3f min( Vector3f::MAX() );
			Vector3f max( Vector3f::MIN() );

			ImageRgbFloat i1( 4, 4 );
			for( dword i = i1.getLength();  i-- > 0; )
			{
				const float a = getRandFp(), b = getRandFp(), c = getRandFp();
				Vector3f v( a, b, c );
				i1.set( i, v *= 1000.0f );
				min.clampToMaxOf( v );
				max.clampToMinOf( v );
			}
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n\n";

			ImageRgbFloat i2( 11, 11 );
			TestBilinearVisitor v1( i2 );

			ImageRgbFloat::visitBilinear( i1, v1, i2.getWidth(), i2.getHeight() );

			for( dword i = i2.getLength();  i-- > 0; )
			{
				const Vector3f a( i2.get(i) >= min );
				const Vector3f b( i2.get(i) <= max );
				isOk_ &= (Vector3f::ONE() == a) & (Vector3f::ONE() == b);

				if( pOut && isVerbose && !isOk_ ) *pOut << i2.get(i) << " ";
			}

			if( pOut && isVerbose && !isOk_ ) *pOut << "\n\n";

			if( pOut ) *pOut << "bounds : " <<
				(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

			isOk &= isOk_;
		}
	}


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace


#endif//TESTING
