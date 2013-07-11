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


#include <ctype.h>
#include <fstream>

#include "exr.hpp"
#include "rgbe.hpp"
#include "png.hpp"
#include "ppm.hpp"
#include "ImageRef.hpp"

#include "ImageFormatter.hpp"   // own header is included last


using namespace p3tonemapper_format;




/// constants
const char ImageFormatter::NO_READ_FORMATTER_EXCEPTION_MESSAGE[] =
	"could not read unrecognized image format";
const char ImageFormatter::NO_WRITE_FORMATTER_EXCEPTION_MESSAGE[] =
	"could not write unrecognized image format";
const char ImageFormatter::FILE_OPEN_EXCEPTION_MESSAGE[] =
	"could not open image file";


static std::string getFileNameExtension
(
	const char filePathname[]
);




/// standard object services ---------------------------------------------------
ImageFormatter::ImageFormatter()
 :	exrLibraryPathName_m()
 ,	pngLibraryPathName_m()
{
}


ImageFormatter::ImageFormatter
(
	const char exrLibraryPathName[],
	const char pngLibraryPathName[]
)
 :	exrLibraryPathName_m()
 ,	pngLibraryPathName_m()
{
	ImageFormatter::setImageFormatter( exrLibraryPathName, pngLibraryPathName );
}


ImageFormatter::~ImageFormatter()
{
}


ImageFormatter::ImageFormatter
(
	const ImageFormatter& other
)
{
	ImageFormatter::operator=( other );
}


ImageFormatter& ImageFormatter::operator=
(
	const ImageFormatter& other
)
{
	if( &other != this )
	{
		exrLibraryPathName_m = other.exrLibraryPathName_m;
		pngLibraryPathName_m = other.pngLibraryPathName_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void ImageFormatter::setImageFormatter
(
	const char exrLibraryPathName[],
	const char pngLibraryPathName[]
)
{
	exrLibraryPathName_m = exrLibraryPathName;
	pngLibraryPathName_m = pngLibraryPathName;
}


void ImageFormatter::setPngLibrary
(
	const char libraryPathName[]
)
{
	pngLibraryPathName_m = libraryPathName;
}


void ImageFormatter::setExrLibrary
(
	const char libraryPathName[]
)
{
	exrLibraryPathName_m = libraryPathName;
}




/// queries --------------------------------------------------------------------
void ImageFormatter::readImage
(
	const char filePathname[],
	ImageRef&  image
) const
{
	// declare image data
	dword  width            = 0;
	dword  height           = 0;
	float  primaries[8];
	float  scalingToGetCdm2 = 0.0f;
	float* pRgbTriples      = 0;

	// choose formatter and read
	{
		// get name ext
		const std::string nameExt( getFileNameExtension( filePathname ) );

		// choose formatter
		// Radiance (rgbe pic hdr rad)
		if( (std::string("hdr") == nameExt) || (std::string("rad") == nameExt) ||
			(std::string("rgbe") == nameExt) || (std::string("pic") == nameExt) )
		{
			float exposure = 0.0f;

			// make in file stream
			std::ifstream inBytes( filePathname, std::ifstream::binary );
			if( !inBytes )
			{
				throw FILE_OPEN_EXCEPTION_MESSAGE;
			}

			// read image file into data
			p3tonemapper_format::rgbe::read( inBytes, false, width, height,
				primaries, exposure, pRgbTriples );

			scalingToGetCdm2 = (exposure != 0.0f) ? 1.0f / exposure : 0.0f;
		}
		// OpenEXR (exr)
		else if( std::string("exr") == nameExt )
		{
			// read image file into data
			p3tonemapper_format::exr::read( exrLibraryPathName_m.c_str(),
				filePathname, 0, width, height, primaries, scalingToGetCdm2,
				pRgbTriples );
		}
		else
		{
			throw NO_READ_FORMATTER_EXCEPTION_MESSAGE;
		}
	}

	// set image
	{
		bool isPrimariesSet = false;
		for( dword i = 8;  i-- > 0; )
		{
			isPrimariesSet |= (0.0f != primaries[i]);
		}

		// set data to image, adopting pixel storage
		image.set( width, height,
			isPrimariesSet ? primaries : 0, scalingToGetCdm2,
			image.PIXELS_FLOAT, pRgbTriples );
	}
}


void ImageFormatter::writeImage
(
	const char      filePathname[],
	const ImageRef& image
) const
{
	// extract image data
	const dword      width         = image.getWidth();
	const dword      height        = image.getHeight();
	const bool       is48Bit       = image.PIXELS_WORD == image.getPixelType();
	const dword      orderingFlags = 0;
	const void*const pRgbTriples   = image.getPixels();

	// make out file stream
	std::ofstream outBytes( filePathname, std::ofstream::binary );

	// get name ext
	const std::string nameExt( getFileNameExtension( filePathname ) );

	// choose formatter
	// ppm
	if( std::string("ppm") == nameExt )
	{
		// write image data to stream
		p3tonemapper_format::ppm::write( width, height, is48Bit, orderingFlags,
			pRgbTriples, outBytes );
	}
	// png
	else if( std::string("png") == nameExt )
	{
		// extract more image data
		const float* pPrimaries8 = image.getPrimaries();
		const float  gamma       = 0.0f;

		// write image data to stream
		p3tonemapper_format::png::write( pngLibraryPathName_m.c_str(),
			width, height, pPrimaries8, gamma,
			is48Bit, orderingFlags, pRgbTriples, outBytes );
	}
	else
	{
		throw NO_WRITE_FORMATTER_EXCEPTION_MESSAGE;
	}
}




/// implementation -------------------------------------------------------------
std::string getFileNameExtension
(
	const char filePathname[]
)
{
	std::string nameExt;

	std::string fpn( filePathname );
	const size_t extPos = fpn.rfind( '.' );
	if( std::string::npos != extPos )
	{
		nameExt = fpn.substr( extPos + 1 );

		// lower case-ify
		// (you would think there is a function for this, but, unbelievably, no.)
		for( dword i = nameExt.length();  i-- > 0; )
		{
			nameExt[i] = char(::tolower( nameExt[i] ));
		}
	}

	return nameExt;
}
