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


#ifndef ImageRgbInt_h
#define ImageRgbInt_h


#include "ColorSpace.hpp"




#include "p3tonemapper_image.hpp"
namespace p3tonemapper_image
{


/**
 * Wrapper for image of RGB integer pixels.<br/><br/>
 *
 * @invariants
 * width_m >= 0
 * height_m >= 0
 * bytesPerChannel_m == 1 or bytesPerChannel_m == 2
 * pPixel3s_m is valid pointer or 0
 * gamma_m is 0 or >= 1e-2 and <= 1e+2
 */
class ImageRgbInt
{
/// standard object services ---------------------------------------------------
public:
	         ImageRgbInt( dword width,
	                      dword height,
	                      bool  is48BitNot24Bit,
	                      bool  isAdopt,
	                      void* pPixel3s );

	virtual ~ImageRgbInt();
	         ImageRgbInt( const ImageRgbInt& );
	ImageRgbInt& operator=( const ImageRgbInt& );


/// commands -------------------------------------------------------------------
	virtual void  setColorSpace( const ColorSpace& );
	/**
	 * give 0.0 for the (partly linear) ITU-R BT.709 transfer function.
	 */
	virtual void  setGamma( float );

	virtual void  setElement( dword        index,
	                          const float* pValue013 );


/// queries --------------------------------------------------------------------
	virtual dword getWidth()                                               const;
	virtual dword getHeight()                                              const;
	virtual dword getLength()                                              const;
	virtual bool  is48Bit()                                                const;
	virtual dword getBytesPerChannel()                                     const;

	virtual const ColorSpace& getColorSpace()                              const;
	virtual float getGamma()                                               const;

	virtual const void* getPixels()                                        const;


/// fields ---------------------------------------------------------------------
private:
	dword width_m;
	dword height_m;
	dword bytesPerChannel_m;

	void* pPixel3s_m;
	bool  isAdopt_m;

	ColorSpace colorSpace_m;
	float      gamma_m;
};


}//namespace




#endif//ImageRgbInt_h
