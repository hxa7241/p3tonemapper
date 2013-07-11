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
#include "ColorSpace.hpp"

#include "AcuityFilter.hpp"   /// own header is included last


using namespace p3tonemapper_tonemap;




/// standard object services ---------------------------------------------------
AcuityFilter::AcuityFilter
(
	const ColorSpace&    inColorSpace,
	const ImageRgbFloat& intermediate,
	ImageRgbFloat&       outImage
)
 :	ImageRgbFloat::BilinearVisitor()
 ,	pInColorSpace_m( &inColorSpace )
 ,	pIntermediate_m( &intermediate )
 ,	pOutImage_m    ( &outImage )
{
}


AcuityFilter::~AcuityFilter()
{
}


AcuityFilter::AcuityFilter
(
	const AcuityFilter& other
)
 :	ImageRgbFloat::BilinearVisitor( other )
 ,	pInColorSpace_m( other.pInColorSpace_m )
 ,	pIntermediate_m( other.pIntermediate_m )
 ,	pOutImage_m    ( other.pOutImage_m )
{
}


AcuityFilter& AcuityFilter::operator=
(
	const AcuityFilter& other
)
{
	if( &other != this )
	{
		ImageRgbFloat::BilinearVisitor::operator=( other );

		pInColorSpace_m = other.pInColorSpace_m;
		pIntermediate_m = other.pIntermediate_m;
		pOutImage_m     = other.pOutImage_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void AcuityFilter::operate
(
	const Vector3f& inValue,
	const dword     outX,
	const dword     outY
)
{
	// convolution only simple implementation, optimize later if needed...

	// calc kernel width
	dword kernelWidth = 1;
	{
		using hxa7241_graphics::ColorConstants::getLuminanceMin;

		// get adaptation luminance
		// clamp luminance to minimum
		float fovealLuminance = pInColorSpace_m->getRgbLuminance( inValue );
		if( fovealLuminance < getLuminanceMin() )
		{
			fovealLuminance = getLuminanceMin();
		}

		// calc acuity at that luminance, in cycles per degree
		// ~2 <= acuity <= ~50 (for human eye)
		static const float ACUITY_MAX = 50.0f;
		float acuity = 17.25f * ::atanf(1.4f * ::log10f(fovealLuminance) +
			0.35f) + 25.72f;
		acuity = acuity > ACUITY_MAX ? ACUITY_MAX : acuity;

		// assume original image is at the resolution equivalent to maximum
		// acuity
		// (it is not known how many degrees of visual field each pixel of it
		// will cover when it is displayed)
		// 'round' into odd integer kernel width
		const float kernelWidthFp = float(ACUITY_MAX) / acuity;
		kernelWidth = (dword(kernelWidthFp * 0.5f) * 2) + 1;
	}

	// calc filtered value
	Vector3f outValue;
	if( kernelWidth <= 1 )
	{
		// just copy
		outValue = pIntermediate_m->get( outX, outY );
	}
	else
	{
		Vector3f sum;
		float    weight = 0.0f;

		// gather under kernel
		const dword halfKernel = kernelWidth / 2;
		for( dword ky = -halfKernel;  ky <= halfKernel;  ++ky )
		{
			for( dword kx = -halfKernel;  kx <= halfKernel;  ++kx )
			{
				// calc scaling
				const float scaling =
					coneFilter( float(kernelWidth) * 0.5f, kx, ky );

				if( scaling > 0.0f )
				{
					const dword okx = outX + kx;
					const dword oky = outY + ky;

					// clamp to image edge
					if( (okx >= 0) &
						 (oky >= 0) &
						 (okx < pOutImage_m->getWidth()) &
						 (oky < pOutImage_m->getHeight()) )
					{
						// accumulate scaled pixel value
						sum += (pIntermediate_m->get( okx, oky ) *= scaling);

						// accumulate non-zero scalings
						weight += scaling;
					}
				}
			}
		}

		// unitize sum
		weight = (0.0f == weight) ? 1.0f : weight;
		outValue = sum / weight;
	}

	// write filtered value to pixel
	pOutImage_m->set( outX, outY, outValue );
}




/// queries --------------------------------------------------------------------




/// implementation -------------------------------------------------------------
float AcuityFilter::coneFilter
(
	float       kernelRadius,
	const dword x,
	const dword y
)
{
	static const float PI = 3.14159265358979323846f;

	if( kernelRadius < 0.0f )
	{
		kernelRadius = -kernelRadius;
	}


	float result;

	const float pointDistance = ::sqrtf( (float(x) * float(x)) +
		(float(y) * float(y)) );

	if( pointDistance < kernelRadius )
	{
		result = (1.0f - (pointDistance / kernelRadius)) /
		         ((1.0f / 3.0f) * PI * kernelRadius * kernelRadius);
	}
	else
	{
		result = 0.0f;
	}

	return result;
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>
#include <math.h>


static dword random_m;

static void setRand( const dword seed )
{
	random_m = seed;
}

static dword getRand()
{
	random_m = dword(1664525) * random_m + dword(1013904223);
	return random_m;
}

static float getRand01()
{
	return float(udword(getRand())) / 4294967296.0f;
}


namespace p3tonemapper_tonemap
{
	using namespace hxa7241;


bool test_AcuityFilter
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_AcuityFilter ]\n\n";


	// flat -> flat
	{
		const dword width  = 100;
		const dword height = 60;
		const Vector3f flatValue( 0.75f, 1.0f, 1.25f );

		setRand( seed );

		// make images
		ImageRgbFloat in( width, height );
		ImageRgbFloat out( width, height );
		ImageRgbFloat temp( width, height );
		{
			float min = FLOAT_MAX;
			float max = FLOAT_MIN_NEG;
			for( dword i = 0;  i < (width * height);  ++i )
			{
				// random luminance over whole range
				const float y = ::powf( 10.0f, (getRand01() * 10.0f) - 4.0f );

				in.set( i, Vector3f( y, y, y ) );
				temp.set( i, flatValue );

				min = min > y ? y : min;
				max = max < y ? y : max;
				//if( pOut && isVerbose ) *pOut << y << " ";
			}
			//if( pOut && isVerbose ) *pOut << "\n\n";
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n\n";
		}

		// do filter
		const ColorSpace colTrans;
		AcuityFilter af( colTrans, temp, out );
		for( dword y = 0;  y < height;  ++y )
		{
			for( dword x = 0;  x < width;  ++x )
			{
				af.operate( in.get(x,y), x, y );
			}
		}

		// check same as input (in temp)
		bool isFail = false;
		{
			float min = FLOAT_MAX;
			float max = FLOAT_MIN_POS;
			for( dword i = 0;  i < (width * height);  ++i )
			{
				const float dif = (out.get(i) - flatValue).abs().largest();
				isFail |= dif > 1e-5f;

				min = min > dif ? dif : min;
				max = max < dif ? dif : max;
				//if( pOut && isVerbose ) *pOut << (out.get(i) - flatValue) << " ";
			}
			//if( pOut && isVerbose ) *pOut << "\n\n";
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "flat : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// exact copy, for non dark
	{
		const dword width  = 100;
		const dword height = 60;
		const Vector3f nonDarkValue( 1.0f, 1.0f, 1.0f );

		setRand( seed );

		// make images
		ImageRgbFloat in( width, height );
		ImageRgbFloat out( width, height );
		ImageRgbFloat temp( width, height );
		{
			Vector3f min( Vector3f::MAX() );
			Vector3f max( Vector3f::MIN() );
			for( dword i = 0;  i < (width * height);  ++i )
			{
				// random value over whole range
				const float x = ::powf( 10.0f, (getRand01() * 10.0f) - 4.0f );
				const float y = ::powf( 10.0f, (getRand01() * 10.0f) - 4.0f );
				const float z = ::powf( 10.0f, (getRand01() * 10.0f) - 4.0f );

				in.set( i, nonDarkValue );
				temp.set( i, Vector3f( x, y, z ) );

				min.clampToMaxOf( temp.get(i) );
				max.clampToMinOf( temp.get(i) );
				//if( pOut && isVerbose ) *pOut << y << " ";
			}
			//if( pOut && isVerbose ) *pOut << "\n\n";
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n\n";
		}

		// do filter
		const ColorSpace colTrans;
		AcuityFilter af( colTrans, temp, out );
		for( dword y = 0;  y < height;  ++y )
		{
			for( dword x = 0;  x < width;  ++x )
			{
				af.operate( in.get(x,y), x, y );
			}
		}

		// check same as input (in temp)
		bool isFail = false;
		{
			float min = FLOAT_MAX;
			float max = FLOAT_MIN_POS;
			for( dword i = 0;  i < (width * height);  ++i )
			{
				const float dif = (out.get(i) - temp.get(i)).abs().largest();
				isFail |= dif > 1e-5f;

				min = min > dif ? dif : min;
				max = max < dif ? dif : max;
				//if( pOut && isVerbose ) *pOut << (out.get(i) - flatValue) << " ";
			}
			//if( pOut && isVerbose ) *pOut << "\n\n";
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "unchanged : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// point spread
	{
		// a single white point -> peak between 1 and 27 wide

		const Vector3f pointValue( 1000.0f, 1000.0f, 1000.0f );

		// make images
		const dword width  = 51;
		const dword height = 31;
		ImageRgbFloat in( width, height );
		ImageRgbFloat out( width, height );
		ImageRgbFloat temp( width, height );

		bool isFail = false;

		dword peakWidthLast = 27;
		for( float i = -4.0f;  i < 6.0f;  i += 0.25f )
		{
			const float level = ::powf( 10.0f, i );
			const Vector3f fovealValue( level, level, level );
			if( pOut && isVerbose ) *pOut << level << "  ";

			// set images
			{
				for( dword i = 0;  i < (width * height);  ++i )
				{
					in.set( i, fovealValue );
					temp.set( i, Vector3f::ZERO() );
				}
				temp.set( (width / 2) + ((height / 2) * width), pointValue );
			}

			// do filter
			const ColorSpace colTrans;
			AcuityFilter af( colTrans, temp, out );
			for( dword y = 0;  y < height;  ++y )
			{
				for( dword x = 0;  x < width;  ++x )
				{
					af.operate( in.get(x,y), x, y );
				}
			}

			// measure width of peak
			{
				Vector3f min( Vector3f::MAX() );
				Vector3f max( Vector3f::MIN() );
				for( dword y = 0;  y < height;  ++y )
				{
					for( dword x = 0;  x < width;  ++x )
					{
						Vector3f coords(
							static_cast<float>(x), static_cast<float>(y), 0.0f );
						if( Vector3f::ZERO() != out.get(x,y) )
						{
							min.clampToMaxOf( coords );
							max.clampToMinOf( coords );
						}
						//if( pOut && isVerbose ) *pOut << out.get(x,y).getY() << " ";
					}
					//if( pOut && isVerbose ) *pOut << "\n";
				}
				//if( pOut && isVerbose ) *pOut << "\n";
				const dword peakWidth  = dword((max - min).getX()) + 1;
				const dword peakHeight = dword((max - min).getY()) + 1;
				if( pOut && isVerbose ) *pOut << peakWidth << "\n";

				// check peak size within bounds
				isFail |= (peakWidth != peakHeight);
				isFail |= (peakWidth < 1)  | (peakWidth  > 27);
				isFail |= (peakHeight < 1) | (peakHeight > 27);

				// check monoticity
				isFail |= (peakWidth > peakWidthLast);

				peakWidthLast = peakWidth;
			}

			// check symmetry
			{
				for( dword y = 0;  y < height;  ++y )
				{
					for( dword x = 0;  x < width;  ++x )
					{
						const Vector3f pixel( out.get(x,y) );

						const Vector3f difX( pixel - out.get(((width - 1) - x), y) );
						const Vector3f difY( pixel - out.get(x, ((height - 1) - y)) );
						const Vector3f difXY( pixel - out.get(((width - 1) - x), ((height - 1) - y)) );

						isFail |= difX.abs().largest()  > 1e-5f;
						isFail |= difY.abs().largest()  > 1e-5f;
						isFail |= difXY.abs().largest() > 1e-5f;
					}
				}
			}
		}
		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "peak : " <<
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
