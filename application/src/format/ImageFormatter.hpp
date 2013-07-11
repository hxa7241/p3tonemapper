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


#ifndef ImageFormatter_h
#define ImageFormatter_h


#include <string>




#include "p3tonemapper_format.hpp"
namespace p3tonemapper_format
{


/**
 * A general interface for image file IO.<br/><br/>
 *
 * Supports OpenEXR and Radiance-RGBE to read, and PNG and PPM to write.
 * <br/><br/>
 *
 * @exceptions queries throw char[] messages, all throw allocation exceptions
 */
class ImageFormatter
{
/// standard object services ---------------------------------------------------
public:
	         ImageFormatter();
	         ImageFormatter( const char exrLibraryPathName[],
	                         const char pngLibraryPathName[] );

	virtual ~ImageFormatter();
	         ImageFormatter( const ImageFormatter& );
	ImageFormatter& operator=( const ImageFormatter& );


/// commands -------------------------------------------------------------------
	virtual void  setImageFormatter( const char exrLibraryPathName[],
	                                 const char pngLibraryPathName[] );
	virtual void  setPngLibrary( const char libraryPathName[] );
	virtual void  setExrLibrary( const char libraryPathName[] );


/// queries --------------------------------------------------------------------
	/**
	 * @filePathname extension must be one of: .exr .rgbe. .pic .hdr .rad
	 */
	virtual void  readImage ( const char filePathname[],
	                          ImageRef&  image )                           const;
	/**
	 * @filePathname extension must be one of: .png .ppm
	 */
	virtual void  writeImage( const char      filePathname[],
	                          const ImageRef& image )                      const;


/// fields ---------------------------------------------------------------------
private:
	std::string exrLibraryPathName_m;
	std::string pngLibraryPathName_m;

	static const char NO_READ_FORMATTER_EXCEPTION_MESSAGE[];
	static const char NO_WRITE_FORMATTER_EXCEPTION_MESSAGE[];
	static const char FILE_OPEN_EXCEPTION_MESSAGE[];
};


}//namespace




#endif//ImageFormatter_h
