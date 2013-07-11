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


#include "Vector3f.hpp"
#include "ColorSpace.hpp"

#include "ColorAdjustment.hpp"   /// own header is included last


using namespace p3tonemapper_tonemap;




/// standard object services ---------------------------------------------------
ColorAdjustment::ColorAdjustment
(
	const ColorSpace& inColorSpace,
	ImageRgbFloat&    outImage
)
 :	ImageRgbFloat::BilinearVisitor()
 ,	pInColorSpace_m( &inColorSpace )
 ,	pOutImage_m    ( &outImage )
 ,	outGrayRgb_m   ()
{
	// make rgb gray with luminance of 1
	const float outGrayLuminance =
		pOutImage_m->getColorSpace().getRgbLuminance( Vector3f::ONE() );
	outGrayRgb_m = Vector3f::ONE() / outGrayLuminance;
}


ColorAdjustment::~ColorAdjustment()
{
}


ColorAdjustment::ColorAdjustment
(
	const ColorAdjustment& other
)
 :	ImageRgbFloat::BilinearVisitor( other )
 ,	pInColorSpace_m( other.pInColorSpace_m )
 ,	pOutImage_m    ( other.pOutImage_m )
 ,	outGrayRgb_m   ( other.outGrayRgb_m )
{
}


ColorAdjustment& ColorAdjustment::operator=
(
	const ColorAdjustment& other
)
{
	if( &other != this )
	{
		ImageRgbFloat::BilinearVisitor::operator=( other );

		pInColorSpace_m = other.pInColorSpace_m;
		pOutImage_m     = other.pOutImage_m;
		outGrayRgb_m    = other.outGrayRgb_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void ColorAdjustment::operate
(
	const Vector3f& inValue,
	const dword     outX,
	const dword     outY
)
{
	static const float PHOTOPIC_LUM_MIN = 5.62f;
	static const float SCOTOPIC_LUM_MAX = 0.00562f;

	// get adaptation luminance
	const float fovealLuminance =
		pInColorSpace_m->getRgbLuminance( inValue );

	// only adjust when non-photopic
	if( fovealLuminance < PHOTOPIC_LUM_MIN )
	{
		const Vector3f out( pOutImage_m->get( outX, outY ) );

		// calc approximate scotopic luminance
		const Vector3f originalXYZ(
			pOutImage_m->getColorSpace().transRgbToXyz_( out ) );
		float          YScotopic = originalXYZ.getY() * (1.33f * (1.0f +
			((originalXYZ.getY() + originalXYZ.getZ()) / originalXYZ.getX()))
			- 1.68f);
		// clamp to remove the small hump at the top of the mesopic part of the
		// graph (it bothers me -- would it cause banding?)
		if( YScotopic > PHOTOPIC_LUM_MIN )
		{
			YScotopic = PHOTOPIC_LUM_MIN;
		}

		// make scotopic value, as gray RGB
		Vector3f adjustedRGB( outGrayRgb_m * YScotopic );

		// maybe interpolate into mesopic color
		if( fovealLuminance >= SCOTOPIC_LUM_MAX )
		{
			// linearly interpolate from gray to original
			const float fraction = (fovealLuminance - SCOTOPIC_LUM_MAX) /
				(PHOTOPIC_LUM_MIN - SCOTOPIC_LUM_MAX);
			adjustedRGB += ((out - adjustedRGB) * fraction);
		}

		// write out value
		pOutImage_m->set( outX, outY, adjustedRGB );
	}
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>
#include <math.h>


namespace p3tonemapper_tonemap
{
	using namespace hxa7241;


bool test_ColorAdjustment
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_ColorAdjustment ]\n\n";


	/*{
		const ColorSpace ct1;
		const Vector3f xyz1( 1.0f, 1.0f, 1.0f );
		const Vector3f rgb1( ct1.transXyzToRgb_( xyz1 ) );

		const Vector3f rgb2( 1.0f, 1.0f, 1.0f );
		const Vector3f xyz2( ct1.transRgbToXyz_( rgb2 ) );

		if( pOut && isVerbose ) *pOut << "xyz " << xyz1 << "  rgb " << rgb1 << "\n";
		if( pOut && isVerbose ) *pOut << "rgb " << rgb2 << "  xyz " << xyz2 << "\n";
		if( pOut && isVerbose ) *pOut << "\n";
	}*/


	// bands
	{
		const dword width  = 10;
		const dword height = 5;
		const Vector3f color( 0.75f, 1.0f, 1.25f );
//		const Vector3f color( 1.0f, 1.0f, 1.0f );
//		const Vector3f color( 0.950456f, 1.0f, 1.08875f );
		const ColorSpace colTrans;

		// make images
		ImageRgbFloat in( width, height );
		ImageRgbFloat out( width, height );
		for( dword y = 0;  y < height;  ++y )
		{
			for( dword x = 0;  x < width;  ++x )
			{
				// fill each row with a different log level of luminance
				// first is photopic, last is scotopic, middle three are mesopic
				Vector3f c( color *
					(5.62f * ::powf(10.0f, (float(-y) + (float(x) / float(width))))) );
				colTrans.transXyzToRgb( c );
				out.set( x, y, c );
				in.set( x, y, c );

				if( pOut && isVerbose ) *pOut << out.get(x,y) << " ";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}
		if( pOut && isVerbose ) *pOut << "\n";

		// make color adjustment
		ColorAdjustment adj( colTrans, out );

		// operate on pixels
		for( dword y = 0;  y < height;  ++y )
		{
			for( dword x = 0;  x < width;  ++x )
			{
				adj.operate( in.get(x,y), x, y );
			}
		}

		// check
		bool isFail1 = false;
		bool isFail2 = false;
		bool isFail3 = false;
		const float pY = colTrans.transRgbToXyz_( out.get(0,0) ).getY();
		const float sY = colTrans.transRgbToXyz_( out.get(0,3) ).getY();
		if( pOut && isVerbose ) *pOut << pY << "  " << sY << "\n\n";

		for( dword y = 0;  y < height;  ++y )
		{
			for( dword x = 0;  x < width;  ++x )
			{
				if( y < 1 )
				{
					// check photopic unchangedness
					//isFail1 |= in.get(x,y) != out.get(x,y);
					isFail1 |= (in.get(x,y) - out.get(x,y)).abs().largest() >= 1e-6;
				}
				else if( y < 4 )
				{
					// check mesopic luminance
					const Vector3f xyz( colTrans.transRgbToXyz_( out.get(x,y) ) );
					isFail2 |= (xyz.getY() < sY) | (xyz.getY() > pY);
				}
				else
				{
					// check scotopic grayness
					const Vector3f& pixel = out.get(x,y);
					const float mean = pixel.average();
					const float xDif = ::fabsf(pixel.getX() - mean);
					const float yDif = ::fabsf(pixel.getY() - mean);

					isFail3 |= (xDif > 1e-5) | (yDif > 1e-5);
				}

				if( pOut && isVerbose ) *pOut << out.get(x,y) <<
					colTrans.transRgbToXyz_( out.get(x,y) ).getY() << " ";
			}
			if( pOut && isVerbose ) *pOut << "\n";
		}
		if( pOut && isVerbose ) *pOut << "\n";

		const bool isFail = isFail1 | isFail2 | isFail3;
		if( pOut && isVerbose ) *pOut << !isFail1 << " " << !isFail2 << " " <<
			!isFail3 << "\n";

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "bands : " <<
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
