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


#ifndef ColorAdjustment_h
#define ColorAdjustment_h


#include "ImageRgbFloat.hpp"
#include "Vector3f.hpp"

#include "hxa7241_graphics.hpp"
#include "p3tonemapper_image.hpp"




#include "p3tonemapper_tonemap.hpp"
namespace p3tonemapper_tonemap
{
	using p3tonemapper_image::ImageRgbFloat;
	using p3tonemapper_image::ColorSpace;
	using hxa7241_graphics::Vector3f;


/**
 * Simple simulation of human color vision loss in dark environments.<br/><br/>
 *
 * Visitor for ImageRgbFloat::visitBilinear.
 *
 * @see
 * p3tonemapper_image::ImageRgbFloat
 *
 * @implementation
 * derived from the paper:
 * <cite>'A Visibility Matching Tone Reproduction Operator for High Dynamic
 * Range Scenes';<br/>
 * Ward Larson, Rushmeier, Piatko;<br/>
 * University Of California 1997.</cite>
 *
 * @invariants
 * pInColorSpace_m is valid
 * pOutImage_m is valid
 */
class ColorAdjustment
	: public ImageRgbFloat::BilinearVisitor
{
/// standard object services ---------------------------------------------------
public:
	         ColorAdjustment( const ColorSpace& inColorSpace,
	                          ImageRgbFloat&    outImage );

	virtual ~ColorAdjustment();
	         ColorAdjustment( const ColorAdjustment& );
	ColorAdjustment& operator=( const ColorAdjustment& );


/// commands -------------------------------------------------------------------
	virtual void  operate( const Vector3f& inValue,
								  dword           outX,
								  dword           outY );


/// queries --------------------------------------------------------------------


/// fields ---------------------------------------------------------------------
private:
	const ColorSpace* pInColorSpace_m;
	ImageRgbFloat*    pOutImage_m;
	Vector3f          outGrayRgb_m;
};


}//namespace




#endif//ColorAdjustment_h
