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


#ifndef AcuityFilter_h
#define AcuityFilter_h


#include "ImageRgbFloat.hpp"

#include "hxa7241_graphics.hpp"
#include "p3tonemapper_image.hpp"




#include "p3tonemapper_tonemap.hpp"
namespace p3tonemapper_tonemap
{
	using p3tonemapper_image::ImageRgbFloat;
	using p3tonemapper_image::ColorSpace;
	using hxa7241_graphics::Vector3f;


/**
 * Simple simulation of human vision acuity loss in dark environments.<br/><br/>
 *
 * Visitor for ImageRgbFloat::visitBilinear.
 *
 * @see
 * p3tonemapper_image::ImageRgbFloat
 *
 * @implementation
 * derived from the paper:
 * 'A Visibility Matching Tone Reproduction Operator for High Dynamic Range
 * Scenes'
 * Ward Larson, Rushmeier, Piatko;
 * University Of California 1997.
 *
 * @invariants
 * pInColorSpace_m is valid
 * pIntermediate_m is valid
 * pOut is valid
 */
class AcuityFilter
	: public ImageRgbFloat::BilinearVisitor
{
/// standard object services ---------------------------------------------------
public:
		AcuityFilter( const ColorSpace&    inColorSpace,
		              const ImageRgbFloat& intermediate,
		              ImageRgbFloat&       outImage );

	virtual ~AcuityFilter();
	         AcuityFilter( const AcuityFilter& );
	AcuityFilter& operator=( const AcuityFilter& );


/// commands -------------------------------------------------------------------
	virtual void  operate( const Vector3f& inValue,
								  dword           outX,
								  dword           outY );


/// queries --------------------------------------------------------------------


/// implementation -------------------------------------------------------------
protected:
	static  float coneFilter( float kernelRadius,
	                          dword x,
	                          dword y );


/// fields ---------------------------------------------------------------------
private:
	const ColorSpace*    pInColorSpace_m;
	const ImageRgbFloat* pIntermediate_m;
	ImageRgbFloat*       pOutImage_m;

//	float (*pFilter_m)(float, dword, dword);
};


}//namespace




#endif//AcuityFilter_h
