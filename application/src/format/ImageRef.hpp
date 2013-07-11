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


#ifndef ImageRef_h
#define ImageRef_h




#include "p3tonemapper_format.hpp"
namespace p3tonemapper_format
{


/**
 * A simple adopting image wrapper.<br/><br/>
 *
 */
class ImageRef
{
public:
	enum EPixelType
	{
		PIXELS_FLOAT,
		PIXELS_BYTE,
		PIXELS_WORD
	};


/// standard object services ---------------------------------------------------
public:
	         ImageRef();
	         ImageRef( dword        width,
	                   dword        height,
	                   const float* pPrimaries8,
	                   float        scaling,
	                   EPixelType   pixelType,
	                   void*        pPixels );
	         ImageRef( const ImageRef& other,
	                   EPixelType      pixelType );

	        ~ImageRef();
private:
	         ImageRef( const ImageRef& );
	ImageRef& operator=( const ImageRef& );
public:


/// commands -------------------------------------------------------------------
	        void  set( dword        width,
	                   dword        height,
	                   const float* pPrimaries8,
	                   float        scaling,
	                   EPixelType   pixelType,
	                   void*        pPixels );


/// queries --------------------------------------------------------------------
	        dword        getWidth()                                        const;
	        dword        getHeight()                                       const;
	        const float* getPrimaries()                                    const;
	        float        getScaling()                                      const;
	        EPixelType   getPixelType()                                    const;
	        void*        getPixels()                                       const;


/// fields ---------------------------------------------------------------------
private:
	dword      width_m;
	dword      height_m;

	bool       isPrimariesSet_m;
	float      primaries8_m[8];

	float      scaling_m;

	EPixelType pixelType_m;
	void*      pPixels_m;
};


}//namespace




#endif//ImageRef_h
