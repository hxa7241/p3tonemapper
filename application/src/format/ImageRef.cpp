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


#include "ImageRef.hpp"   // own header is included last


using namespace p3tonemapper_format;




/// standard object services ---------------------------------------------------
ImageRef::ImageRef()
{
	ImageRef::set( 0, 0, 0, 0.0f, PIXELS_FLOAT, 0 );
}


ImageRef::ImageRef
(
	const ImageRef&  other,
	const EPixelType pixelType
)
{
	void*       pPixels = 0;
	const dword length  = other.width_m * other.height_m;
	switch( pixelType )
	{
		case PIXELS_FLOAT :
			pPixels = new float[ length * 3 ];
			break;

		case PIXELS_BYTE :
			pPixels = new ubyte[ length * 3 ];
			break;

		case PIXELS_WORD :
			pPixels = new uword[ length * 3 ];
			break;
	}

	ImageRef::set( other.width_m, other.height_m,
		other.getPrimaries(), other.scaling_m,
		pixelType, pPixels );
}


ImageRef::ImageRef
(
	const dword      width,
	const dword      height,
	const float*     pPrimaries8,
	const float      scaling,
	const EPixelType pixelType,
	void*const       pPixels
)
{
	ImageRef::set( width, height, pPrimaries8, scaling, pixelType, pPixels );
}


ImageRef::~ImageRef()
{
	switch( pixelType_m )
	{
		case PIXELS_FLOAT :
			delete[] static_cast<float*>( pPixels_m );
			break;

		case PIXELS_BYTE :
			delete[] static_cast<ubyte*>( pPixels_m );
			break;

		case PIXELS_WORD :
			delete[] static_cast<uword*>( pPixels_m );
			break;
	}
}


//ImageRef::ImageRef
//(
//	const ImageRef& other
//)
//{
//	ImageRef::operator=( other );
//}
//
//
//ImageRef& ImageRef::operator=
//(
//	const ImageRef& other
//)
//{
//	if( &other != this )
//	{
//		set( other.width_m, other.height_m, other.getPrimaries(), other.scaling_m,
//			other.pixelType_m, other.pPixels_m );
//	}
//
//	return *this;
//}




/// commands -------------------------------------------------------------------
void ImageRef::set
(
	const dword      width,
	const dword      height,
	const float*     pPrimaries8,
	const float      scaling,
	const EPixelType pixelType,
	void*const       pPixels
)
{
	width_m  = width;
	height_m = height;

	isPrimariesSet_m = (0 != pPrimaries8);
	for( dword i = 8;  i-- > 0; )
	{
		primaries8_m[i] = isPrimariesSet_m ? pPrimaries8[i] : 0.0f;
	}

	scaling_m = scaling;

	pixelType_m = pixelType;
	pPixels_m   = pPixels;
}




/// queries --------------------------------------------------------------------
dword ImageRef::getWidth() const
{
	return width_m;
}


dword ImageRef::getHeight() const
{
	return height_m;
}


const float* ImageRef::getPrimaries() const
{
	return isPrimariesSet_m ? primaries8_m : 0;
}


float ImageRef::getScaling() const
{
	return scaling_m;
}


ImageRef::EPixelType ImageRef::getPixelType() const
{
	return pixelType_m;
}


void* ImageRef::getPixels() const
{
	return pPixels_m;
}
