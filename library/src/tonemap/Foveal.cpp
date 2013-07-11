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
#include "ImageRgbFloatIter.hpp"

#include "Foveal.hpp"   // own header is included last


using namespace p3tonemapper_tonemap;




/// standard object services ---------------------------------------------------
Foveal::Foveal
(
	const ImageRgbFloat& imageSource
)
 :	ImageRgbFloat()
{
	Foveal::construct( imageSource, 65.0f );
}


Foveal::Foveal
(
	const ImageRgbFloat& imageSource,
	const float          viewAngleHorizontal
)
 :	ImageRgbFloat()
{
	Foveal::construct( imageSource, viewAngleHorizontal );
}


Foveal::~Foveal()
{
}


Foveal::Foveal
(
	const Foveal& other
)
 :	ImageRgbFloat( other )
{
}


Foveal& Foveal::operator=
(
	const Foveal& other
)
{
	ImageRgbFloat::operator=( other );

	return *this;
}




/// commands -------------------------------------------------------------------




/// queries --------------------------------------------------------------------




/// implementation -------------------------------------------------------------
void Foveal::construct
(
	const ImageRgbFloat& imageSource,
	const float          viewAngleHorizontal
)
{
	// calculate size
	dword widthFoveal;
	dword heightFoveal;
	Foveal::calcSize( imageSource, viewAngleHorizontal,
	                  widthFoveal, heightFoveal );

	// set image basics
	ImageRgbFloat::setImage( widthFoveal, heightFoveal );
	ImageRgbFloat::setColorSpace( imageSource.getColorSpace() );

	// scale and copy pixels
	Foveal::scale( imageSource, widthFoveal, *this );
}


void Foveal::calcSize
(
	const ImageRgbFloat& imageSource,
	float                viewAngleHorizontal,
	dword&               width,
	dword&               height
)
{
	// minimum: 1 degree == 1 pixel
	if( viewAngleHorizontal < 1.0f )
	{
		viewAngleHorizontal = 1.0f;
	}
	// maximum: 120 degree == 198 pixel
	else if( viewAngleHorizontal > 120.0f )
	{
		viewAngleHorizontal = 120.0f;
	}

	static const float ONE_DEGREE_AS_RADIANS = 0.01745329251994f;
	width = dword( 0.5f +
		(2.0f * ::tanf( viewAngleHorizontal * ONE_DEGREE_AS_RADIANS / 2.0f )
		/ ONE_DEGREE_AS_RADIANS) );

	// allow smaller size than source
	if( width < imageSource.getWidth() )
	{
		// calc height from aspect ratio
		height = dword( float(width) *
			float(imageSource.getHeight()) / float(imageSource.getWidth()) );
	}
	// disallow larger size than source
	else
	{
		// copy source size
		width  = imageSource.getWidth();
		height = imageSource.getHeight();
	}
}


void Foveal::scale
(
	const ImageRgbFloat& imageSource,
	const dword          widthFoveal,
	ImageRgbFloat&       imageFoveal
)
{
	// precondition: foveal <= source size

//	// calc height from aspect ratio
//	const dword heightFoveal = dword( float(widthFoveal) *
//		float(imageSource.getHeight()) / float(imageSource.getWidth()) );

	// shrink, if smaller than source
	if( widthFoveal < imageSource.getWidth() )
	{
		// non-integer size box filter.
		// separated into two passes: horizontal scaling, and vertical scaling,
		// through an intermediate temporary image.

		// simple and un-optimized both algorithmically and code-wise --
		// it just needs to weight pixels similarly, not be high quality or fast.

		ImageRgbFloat imageTemp( imageFoveal.getWidth(), imageSource.getHeight() );

		// do two passes
		for( dword pass = 0;  pass < 2;  ++pass )
		{
			using hxa7241_graphics::Vector3f;
			using p3tonemapper_image::ImageRgbFloatIter;
			using p3tonemapper_image::ImageRgbFloatIterConst;

			// for horizontal pass, first
			ImageRgbFloatIterConst pSourcePixel( imageSource.getIteratorConst() );
			ImageRgbFloatIter      pTargetPixel( imageTemp.getIterator() );
			dword                  sourceSize   = imageSource.getWidth();
			dword                  targetSize   = imageTemp.getWidth();
			dword                  sourceStep   = 0;
			dword                  targetStep   = 0;
			dword                  sourceInc    = 1;
			dword                  targetInc    = 1;
			ImageRgbFloatIter   pTargetEnd( pTargetPixel + imageTemp.getLength() );

			// for vertical pass, second
			if( 1 == pass )
			{
				pSourcePixel = imageTemp.getIteratorConst();
				pTargetPixel = imageFoveal.getIterator();
				sourceSize   = imageTemp.getHeight();
				targetSize   = imageFoveal.getHeight();
				sourceStep   = 1 - imageTemp.getLength();
				targetStep   = 1 - imageFoveal.getLength();
				sourceInc    = imageTemp.getWidth();
				targetInc    = imageFoveal.getWidth();
				pTargetEnd   = pTargetPixel + targetInc;
			}

			const float oneOverKernelSize = float(targetSize) / float(sourceSize);

			// step along sets (rows or columns)
			for( ;  pTargetPixel < pTargetEnd;
			     pSourcePixel += sourceStep, pTargetPixel += targetStep )
			{
				dword    sourcePixel = 0;
				Vector3f lastPixelPart;

				// loop through pixels in target set
				for( dword targetPixel = 0;  targetPixel < targetSize;  ++targetPixel )
				{
					Vector3f kernelSum( lastPixelPart );

					// calc loop end with an expression that will not drift, and will
					// give the exactly correct last source pixel
					const float endKernel = (float(targetPixel + 1) /
						float(targetSize)) * float(sourceSize);
					const dword endPixel  = dword(endKernel);
					// loop through kernel
					for( ;  sourcePixel < endPixel;  ++sourcePixel )
					{
						// add source pixel to kernel sum
						kernelSum    += pSourcePixel.get();
						pSourcePixel += sourceInc;
					}

					if( sourcePixel < sourceSize )
					{
						// calc fractions of end pixel
						const float lastPixelFraction = endKernel - float(endPixel);
						kernelSum    += (pSourcePixel.get() *= lastPixelFraction);
						lastPixelPart = (pSourcePixel.get() *= (1.0f - lastPixelFraction));

						pSourcePixel += sourceInc;
						++sourcePixel;
					}

					// unitize kernel sum, and write to target pixel
					pTargetPixel.set( kernelSum * oneOverKernelSize );
					pTargetPixel += targetInc;
				}
			}
		}
	}
	// copy with no scaling
	else
	{
		imageFoveal = imageSource;
	}
}




// non-separated 2d kernel, notes:

	// step along y
		// start and end y: floor source y, ceil source y + kernel size y
		// kernel height: end y - start y
		// col end scalings: ceil - y, y + kernel size y - floor

		// step along x
			// start and end x: floor source x, ceil source x + kernel size x
			// kernel width: end x - start x
			// row end scalings: ceil - x, x + kernel size x - floor

			// (kernel width and height int, start source pixel x and y int,
			//  row end scalings, col end scalings)

			// do kernel
				// zero kernel sum
				// step along y
					// zero row sum
					// step along x
						// scale first and last source pixels
						// add source pixel to row sum
					// scale first and last row sum
					// add row sum to kernel sum
				// divide kernel sum
				// write kernel sum to target pixel








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>


using hxa7241_graphics::Vector3f;


static bool isDiffTolerable
(
	const Vector3f a,
	const Vector3f b
)
{
	bool isOk = true;

	static const float TOLERANCE = 1e-4f;

	float as[3];
	float bs[3];
	a.getXYZ( as );
	b.getXYZ( bs );

	for( dword i = 3;  i-- > 0; )
	{
		if( 0.0f != as[i] )
		{
			isOk &= (::fabsf( bs[i] - as[i] ) / as[i]) < TOLERANCE;
		}
		else
		{
			isOk &= ::fabsf( bs[i] - as[i] ) < TOLERANCE;
		}
	}

	return isOk;
}


namespace p3tonemapper_tonemap
{
	using namespace hxa7241;


bool test_Foveal
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_Foveal ]\n\n";


	// flat
	{
		const Vector3f value( 128.0f, 64.0f, 1.25f );

		// make all same value image
		ImageRgbFloat imageOriginal( 311, 211 );
		{
			for( dword i = imageOriginal.getLength();  i-- > 0; )
			{
				imageOriginal.set( i, value );
			}
		}

		// shrink image
		Foveal imageFoveal( imageOriginal, 63.5f );

		// check that result image is all same value
		bool isFail = false;
		{
			Vector3f min( Vector3f::MAX() );
			Vector3f max( Vector3f::MIN() );
			for( dword i = imageFoveal.getLength();  i-- > 0; )
			{
				const Vector3f fp( imageFoveal.get(i) );

				isFail |= !isDiffTolerable( value, fp );

				min.clampToMaxOf( fp );
				max.clampToMinOf( fp );
			}
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n\n";
		}

		if( pOut ) *pOut << "flat : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// border
	{
		const Vector3f value( 128.0f, 128.0f, 128.0f );

		// draw 1 pixel gray border on black image
		ImageRgbFloat imageOriginal( 311, 211 );
		{
			const dword width  = imageOriginal.getWidth();
			const dword height = imageOriginal.getHeight();
			for( dword y = 0;  y < height;  ++y )
			{
				for( dword x = 0;  x < width;  ++x )
				{
					if( (0 == x) | (0 == y) | (width-1 == x) | (height-1 == y) )
					{
						imageOriginal.set( x, y, value );
					}
					else
					{
						imageOriginal.set( x, y, Vector3f::ZERO() );
					}
				}
			}
		}

		// shrink image
		Foveal imageFoveal( imageOriginal, 63.5f );

		// check that result image has 1 pixel gray border on black
		bool isFail = false;
		{
			Vector3f min( Vector3f::MAX() );
			Vector3f max( Vector3f::MIN() );

			const dword width  = imageFoveal.getWidth();
			const dword height = imageFoveal.getHeight();
			for( dword y = 0;  y < height;  ++y )
			{
				for( dword x = 0;  x < width;  ++x )
				{
					const Vector3f rgb( imageFoveal.get( x, y ) );
					if( (0 == x) | (0 == y) | (width-1 == x) | (height-1 == y) )
					{
						// all channels same
						isFail |= (rgb.getX() != rgb.getY());
						isFail |= (rgb.getY() != rgb.getZ());
						// 0 < channel <= value
						isFail |= (rgb.getX() < 0.0f);
						isFail |= (rgb.getX() >= value.getX());

						min.clampToMaxOf( rgb );
						max.clampToMinOf( rgb );
					}
					else
					{
						isFail |= !rgb.isZero();
					}
				}
			}
			if( pOut && isVerbose ) *pOut << min << "  " << max << "\n\n";
		}

		if( pOut ) *pOut << "border : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// ramps
	{
		// horizontal ramp, then vertical
		for( dword ramp = 0;  ramp < 2;  ++ramp )
		{
			// make image of horiz/vert ramp from black to (128,64,1.25 rgb)
			ImageRgbFloat imageOriginal( 311, 211 );
			{
				const dword width  = imageOriginal.getWidth();
				const dword height = imageOriginal.getHeight();

				for( dword y = 0;  y < height;  ++y )
				{
					for( dword x = 0;  x < width;  ++x )
					{
						Vector3f value( 128.0f, 64.0f, 1.25f );
						if( 0 == ramp )
						{
							value *= float(x) / float(width);
						}
						else
						{
							value *= float(y) / float(height);
						}

						imageOriginal.set( x, y, value );
					}
				}
			}

			// shrink image
			Foveal imageFoveal( imageOriginal, 63.5f );

			// check that result image is a horizontal/vertical ramp
			bool isFail = false;
			{
				// all pixels in each column/row are the same
				// each horiz/vert adjacent pixel is greater value than last

				const dword width  = imageFoveal.getWidth();
				const dword height = imageFoveal.getHeight();

				for( dword y = 0;  y < height;  ++y )
				{
					// check rows
					Vector3f last( imageFoveal.get( 0, y ) );
					for( dword x = 1;  x < width;  ++x )
					{
						//if( pOut && isVerbose ) *pOut << last.getX() << " ";

						Vector3f current( imageFoveal.get( x, y ) );
						if( 0 == ramp )
						{
							isFail |= (current <= last).sum() > 0.0f;
						}
						else
						{
							isFail |= !isDiffTolerable( last, current );
						}
						last = current;
					}
					//if( pOut && isVerbose ) *pOut << last.getX() << "\n";
				}

				for( dword x = 0;  x < width;  ++x )
				{
					// check columns
					Vector3f last( imageFoveal.get( x, 0 ) );
					for( dword y = 1;  y < height;  ++y )
					{
						Vector3f current( imageFoveal.get( x, y ) );
						if( 0 == ramp )
						{
							isFail |= !isDiffTolerable( last, current );
						}
						else
						{
							isFail |= (current <= last).sum() > 0.0f;
						}
						last = current;
					}
				}
			}
			//if( pOut && isVerbose ) *pOut << "\n";

			isOk &= !isFail;
		}

		if( pOut ) *pOut << "ramps : " <<
			(isOk ? "--- succeeded" : "*** failed") << "\n\n";
	}


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace


#endif//TESTING
