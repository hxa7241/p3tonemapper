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


#ifndef Veil_h
#define Veil_h


#include "ImageRgbFloat.hpp"




#include "p3tonemapper_tonemap.hpp"
namespace p3tonemapper_tonemap
{
	using p3tonemapper_image::ImageRgbFloat;


/**
 * An image of the light scattered internally by the human eye.<br/><br/>
 *
 * Calculated from a foveal image. When mixed back into the image it was
 * derived from, it effectively spreads the light around a bit.
 *
 * @exceptions constructor can throw
 *
 * @see
 * Foveal
 * p3tonemapper_image::ImageRgbFloat
 *
 * @implementation
 * Construction convolution and mixInto interpolation are simple and
 * un-optimised. The convolution is a likely candidate for speeding up...
 */
class Veil
	: public ImageRgbFloat
{
/// standard object services ---------------------------------------------------
public:
	explicit Veil( const Foveal& );

	virtual ~Veil();
	         Veil( const Veil& );
	Veil& operator=( const Veil& );


/// commands -------------------------------------------------------------------
	// inherit


/// queries --------------------------------------------------------------------
	// inherit

	virtual void  mixInto( ImageRgbFloat& )                                const;


/// implementation -------------------------------------------------------------
protected:
	static  void  doBigConvolution( const Foveal&,
	                                Veil& );


/// fields ---------------------------------------------------------------------
private:
	static const float CENTRAL_WEIGHTING;
	static const float PERIPHERAL_WEIGHTING;
};


}//namespace




#endif//Veil_h
