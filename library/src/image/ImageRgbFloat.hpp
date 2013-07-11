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


#ifndef ImageRgbFloat_h
#define ImageRgbFloat_h


#include "Sheet.hpp"
#include "ColorSpace.hpp"




#include "p3tonemapper_image.hpp"
namespace p3tonemapper_image
{
	using hxa7241_general::Sheet;
	using hxa7241_graphics::Vector3f;


/**
 * Image of high dynamic range fp RGB pixels.<br/><br/>
 *
 * Not really only RGB. Is essentially any colorspace triplet 2D array --
 * since a CIE XYZ color conversion is included.<br/><br/>
 *
 * Non-default constructors and setSize can throw.<br/><br/>
 *
 * @see
 * ImageRgbFloatIter
 * ImageRgbFloatIterConst
 * ColorSpace
 * hxa7241_general::Sheet
 * hxa7241_graphics::Vector3f
 *
 * @invariants
 * sheet_m values >= 0 and <= FLOAT_LARGE
 */
class ImageRgbFloat
{
/// standard object services ---------------------------------------------------
public:
	         ImageRgbFloat();
	         ImageRgbFloat( dword width,
	                        dword height );
	         ImageRgbFloat( dword             width,
	                        dword             height,
	                        float*            pInPixel3s,
	                        bool              isAdopt,
	                        const ColorSpace& colorSpace );

	virtual ~ImageRgbFloat();
	         ImageRgbFloat( const ImageRgbFloat& );
	ImageRgbFloat& operator=( const ImageRgbFloat& );


/// commands -------------------------------------------------------------------
	virtual void  setImage( dword width,
	                        dword height );
	virtual void  setImage( dword             width,
	                        dword             height,
	                        float*            pInPixel3s,
	                        bool              isAdopt,
	                        const ColorSpace& colorSpace );

	virtual void  setColorSpace( const ColorSpace& );

	virtual void  set( dword x,
	                   dword y,
	                   const Vector3f& );
	virtual void  set( dword i,
	                   const Vector3f& );

	virtual ImageRgbFloatIter getIterator();

	virtual void  zeroValues();
	virtual void  clampValues();


/// queries --------------------------------------------------------------------
	virtual dword getLength()                                              const;
	virtual dword getWidth()                                               const;
	virtual dword getHeight()                                              const;
	virtual bool  isOwning()                                               const;

	virtual const ColorSpace& getColorSpace()                              const;

	virtual void     get( dword x,
	                      dword y,
	                      Vector3f& )                                      const;
	virtual Vector3f get( dword x,
	                      dword y )                                        const;
	virtual void     get( dword i,
	                      Vector3f& )                                      const;
	virtual Vector3f get( dword i )                                        const;

	virtual ImageRgbFloatIterConst getIteratorConst()                      const;


/// statics --------------------------------------------------------------------
	static  bool  isSizeWithinRange( dword width,
	                                 dword height );
	static  dword getMaxSize();

	struct BilinearVisitor
	{
		virtual void  operate( const Vector3f& interpolatedPixel,
		                       dword           outX,
		                       dword           outY )                          =0;
	};

	static  void  visitBilinear( const ImageRgbFloat&,
	                             BilinearVisitor&,
	                             dword outWidth,
	                             dword outHeight );


/// implementation -------------------------------------------------------------
protected:
	virtual void  assign( const ImageRgbFloat& );


/// fields ---------------------------------------------------------------------
private:
	Sheet<float> sheet_m;

	ColorSpace   colorSpace_m;
};


}//namespace




#endif//ImageRgbFloat_h
