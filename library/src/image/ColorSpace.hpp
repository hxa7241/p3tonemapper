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


#ifndef ColorSpace_h
#define ColorSpace_h


#include "Vector3f.hpp"
#include "Matrix3f.hpp"




#include "p3tonemapper_image.hpp"
namespace p3tonemapper_image
{
	using hxa7241_graphics::Vector3f;
	using hxa7241_graphics::Matrix3f;


class ColorSpace
{
/// standard object services ---------------------------------------------------
public:
	         ColorSpace();
	         /**
	          * @pChromaticities32
	          * which way around is the chromaticities array? -- the sRGB ones
	          * would be:<pre>
	          * {
	          *    // x    y
	          *    0.64, 0.33,   // r
	          *    0.30, 0.60,   // g
	          *    0.15, 0.06    // b
	          * }</pre><br/><br/>
	          *
	          * each value is [0,1]
	          *
	          * @pWhitePoint2 each value is (0,1)
	          *
	          * @exceptions throws if chromaticities or whitepoint invalid
	          */
	         ColorSpace( const float* pChromaticities32,
	                     const float* pWhitePoint2 );

	        ~ColorSpace();
	         ColorSpace( const ColorSpace& );
	ColorSpace& operator=( const ColorSpace& );


/// commands -------------------------------------------------------------------
	        void  setSrgb();
	        void  setXyz();
	        /**
	         * @pChromaticities32
	         * which way around is the chromaticities array? -- the sRGB ones
	         * would be:<pre>
	         * {
	         *    // x    y
	         *    0.64, 0.33,   // r
	         *    0.30, 0.60,   // g
	         *    0.15, 0.06    // b
	         * }</pre><br/><br/>
	         *
	         * each value is [0,1]
	         *
	         * @pWhitePoint2 each value is (0,1)
	         *
	         * @exceptions throws if chromaticities or whitepoint invalid
	         */
	        void  set( const float* pChromaticities32,
	                   const float* pWhitePoint2 );


/// queries --------------------------------------------------------------------
	        void  transXyzToRgb( Vector3f& xyz )                           const;
	        void  transRgbToXyz( Vector3f& rgb )                           const;

	        Vector3f transXyzToRgb_( const Vector3f& xyz )                 const;
	        Vector3f transRgbToXyz_( const Vector3f& rgb )                 const;

	        float getRgbLuminance( const Vector3f& rgb )                   const;

	        const float* getChromaticities()                               const;
	        const float* getWhitePoint()                                   const;


/// implementation -------------------------------------------------------------
protected:
	        void  setPrimaries( const float* pChromaticities32,
	                            const float* pWhitePoint2 );


/// fields ---------------------------------------------------------------------
private:
	float    chromaticities32_m[6];
	float    whitePoint2_m[2];

   Matrix3f XyzToRgb_m;
   Matrix3f RgbToXyz_m;
};


}//namespace




#endif//ColorSpace_h
