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


#ifndef Foveal_h
#define Foveal_h


#include "ImageRgbFloat.hpp"




#include "p3tonemapper_tonemap.hpp"
namespace p3tonemapper_tonemap
{
	using p3tonemapper_image::ImageRgbFloat;


/**
 * An image with pixels of (usually) 1 degree diameter solid angle.<br/><br/>
 *
 * The human eye adapts for the best view at the fovea, which is 1 degree of
 * the visual field.<br/><br/>
 *
 * If the source image this is generated from is lower resolution than 1
 * degree per pixel, this image is just a copy, and >1 degree per pixel.
 *
 * @exceptions constructors can throw
 *
 * @see
 * p3tonemapper_image::ImageRgbFloat
 *
 * @implementation
 * Scaling method is simple and un-optimized -- it just needs to weight pixels
 * similarly, not be high quality or fast.
 */
class Foveal
	: public ImageRgbFloat
{
/// standard object services ---------------------------------------------------
public:
	explicit Foveal( const ImageRgbFloat& );
	         Foveal( const ImageRgbFloat&,
	                 float viewFrustrumHorizontalAngleDegrees );

	virtual ~Foveal();
	         Foveal( const Foveal& );
	Foveal& operator=( const Foveal& );


/// commands -------------------------------------------------------------------
	// inherit


/// queries --------------------------------------------------------------------
	// inherit


/// implementation -------------------------------------------------------------
protected:
	        void  construct( const ImageRgbFloat&,
	                         float viewAngleHorizontal );

	static  void  calcSize( const ImageRgbFloat& imageSource,
	                        float                viewAngleHorizontal,
	                        dword&               width,
	                        dword&               height );
	static  void  scale( const ImageRgbFloat& imageSource,
	                     dword                widthFoveal,
	                     ImageRgbFloat&       imageFoveal );


/// fields ---------------------------------------------------------------------
private:
};


}//namespace




#endif//Foveal_h
