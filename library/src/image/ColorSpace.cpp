/*--------------------------------------------------------------------

   Perceptuum3 rendering components
   Copyright (c) 2005-2007, Harrison Ainsworth / HXA7241.

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


#include "ColorConstants.hpp"

#include "ColorSpace.hpp"


using namespace p3tonemapper_image;




/// statics
static const char INVALID_CHROMATICITIES_MESSAGE[] =
   "invalid chromaticities given to ColorSpace set";
static const char INVALID_WHITEPOINT_MESSAGE[] =
   "invalid whitepoint given to ColorSpace set";
static const char INVALID_COLORSPACE_MESSAGE[] =
   "invalid colorspace given to ColorSpace set";




/// standard object services ---------------------------------------------------
ColorSpace::ColorSpace()
 : XyzToRgb_m()
 , RgbToXyz_m()
{
   ColorSpace::setSrgb();
}


ColorSpace::ColorSpace
(
   const float* pChromaticities32,
   const float* pWhitePoint2
)
 : XyzToRgb_m()
 , RgbToXyz_m()
{
   ColorSpace::set( pChromaticities32, pWhitePoint2 );
}


ColorSpace::~ColorSpace()
{
}


ColorSpace::ColorSpace
(
   const ColorSpace& other
)
 : XyzToRgb_m()
 , RgbToXyz_m()
{
   ColorSpace::operator=( other );
}


ColorSpace& ColorSpace::operator=
(
   const ColorSpace& other
)
{
   if( &other != this )
   {
      setPrimaries( other.chromaticities32_m, other.whitePoint2_m );

      XyzToRgb_m = other.XyzToRgb_m;
      RgbToXyz_m = other.RgbToXyz_m;
   }

   return *this;
}




/// commands -------------------------------------------------------------------
void ColorSpace::setSrgb()
{
   using namespace hxa7241_graphics::ColorConstants;

   set( getSrgbChromaticities(), getSrgbWhitePoint() );
}


void ColorSpace::setXyz()
{
   for( dword i = 6;  i-- > 0; )
   {
      chromaticities32_m[i] = 0.0f;
   }
   chromaticities32_m[0] = 1.0f;
   chromaticities32_m[3] = 1.0f;
   whitePoint2_m[0] = 0.333333f;
   whitePoint2_m[1] = 0.333333f;

   XyzToRgb_m = Matrix3f::IDENTITY();
   RgbToXyz_m = Matrix3f::IDENTITY();
}


void ColorSpace::set
(
   const float*const pChromaticities32,
   const float*const pWhitePoint2
)
{
   // copy primaries
   setPrimaries( pChromaticities32, pWhitePoint2 );

   // make chromaticities matrix
   Matrix3f chrm;
   {
      Vector3f cvs[3];
      for( dword i = 3;  i-- > 0; )
      {
         const float x = pChromaticities32[i * 2 + 0];
         const float y = pChromaticities32[i * 2 + 1];

         if( (x < 0.0f) | (x > 1.0f) | (y < 0.0f) | (y > 1.0f) )
         {
            throw INVALID_CHROMATICITIES_MESSAGE;
         }

         cvs[i].setXYZ( x, y, 1.0f - (x + y) );
      }

      chrm.setColumns( cvs[0], cvs[1], cvs[2], Vector3f::ZERO() );
   }

   // make white color vector
   Vector3f whiteColor;
   {
      const float x = pWhitePoint2[0];
      const float y = pWhitePoint2[1];

      if( (x < FLOAT_EPSILON) | (x >= 1.0f) |
         (y < FLOAT_EPSILON) | (y >= 1.0f) )
      {
         throw INVALID_WHITEPOINT_MESSAGE;
      }

      // check special middle case -- to make identity transform exact
      if( (x == y) & ((0.333f == y) | (0.333333f == y) | ((1.0f/3.0f) == y)) )
      {
         whiteColor = Vector3f::ONE();
      }
      else
      {
         whiteColor.setXYZ( x, y, 1.0f - (x + y) );

         whiteColor /= whiteColor.getY();
      }
   }

   // start matrix with chromaticities
   RgbToXyz_m = chrm;

   // inverted chromaticities * white color to calculate the unknown
   if( !chrm.invert() )
   {
      throw INVALID_CHROMATICITIES_MESSAGE;
   }
   Vector3f c;
   chrm.multiply( whiteColor, c );

   // scaled chrms makes the conversion matrix
   RgbToXyz_m.scale( c );

   // inverse conversion is the same, but inverted
   XyzToRgb_m = RgbToXyz_m;
   if( !XyzToRgb_m.invert() )
   {
      throw INVALID_COLORSPACE_MESSAGE;
   }
}




/// queries --------------------------------------------------------------------
void ColorSpace::transXyzToRgb
(
   Vector3f& xyz
) const
{
   XyzToRgb_m.multiply( xyz );
}


void ColorSpace::transRgbToXyz
(
   Vector3f& rgb
) const
{
   RgbToXyz_m.multiply( rgb );
}


Vector3f ColorSpace::transXyzToRgb_
(
   const Vector3f& xyz
) const
{
   return XyzToRgb_m.multiply( xyz );
}


Vector3f ColorSpace::transRgbToXyz_
(
   const Vector3f& rgb
) const
{
   return RgbToXyz_m.multiply( rgb );
}


float ColorSpace::getRgbLuminance
(
   const Vector3f& rgb
) const
{
   return rgb.dot( RgbToXyz_m.getRow1() );
}


const float* ColorSpace::getChromaticities() const
{
   return chromaticities32_m;
}


const float* ColorSpace::getWhitePoint() const
{
   return whitePoint2_m;
}




/// implementation -------------------------------------------------------------
void ColorSpace::setPrimaries
(
   const float*const pChromaticities32,
   const float*const pWhitePoint2
)
{
   for( dword i = 6;  i-- > 0; )
   {
      chromaticities32_m[i] = pChromaticities32[i];
   }

   whitePoint2_m[0] = pWhitePoint2[0];
   whitePoint2_m[1] = pWhitePoint2[1];
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <math.h>
#include <iostream>


namespace p3tonemapper_image
{
   using namespace hxa7241;


bool test_ColorSpace
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   //seed
)
{
   bool isOk = true;

   if( pOut ) *pOut << "[ test_ColorSpace ]\n\n";


   // constants comparison
   {
      bool isOk_ = true;

      // from http://www.poynton.com/ color faq
      static const Vector3f RGB_OF_XYZ111(  1.204794f,  0.948292f,  0.908916f );
      static const Vector3f RGB_OF_XYZ100(  3.240479f, -0.969256f,  0.055648f );
      static const Vector3f RGB_OF_XYZ010( -1.537150f,  1.875992f, -0.204043f );
      static const Vector3f RGB_OF_XYZ001( -0.498535f,  0.041556f,  1.057331f );
      static const Vector3f TOLERANCE( 1e-4f, 1e-4f, 1e-4f );

      static const Vector3f xyzs[] = {
         Vector3f::ONE(), Vector3f::X(), Vector3f::Y(), Vector3f::Z() };
      static const Vector3f rgbs[] = {
         RGB_OF_XYZ111, RGB_OF_XYZ100, RGB_OF_XYZ010, RGB_OF_XYZ001 };

      ColorSpace ct1;
      ct1.setSrgb();

      for( udword i = 0;  i < sizeof(xyzs)/sizeof(xyzs[0]);  ++i )
      {
         // forward to RGB
         const Vector3f rgb( ct1.transXyzToRgb_( xyzs[i] ) );
         const Vector3f dif1( (rgb - rgbs[i]).absEq() );
         isOk_ &= (dif1 < TOLERANCE) == Vector3f::ONE();

         if( pOut && isVerbose ) *pOut << rgb << "  " << rgbs[i] << "  " <<
            dif1 << "  " << isOk_ << "\n";

         // back to XYZ
         const Vector3f xyz( ct1.transRgbToXyz_( rgb ) );
         const Vector3f dif2( (xyz - xyzs[i]).absEq() );
         isOk_ &= (dif2 < TOLERANCE) == Vector3f::ONE();

         if( pOut && isVerbose ) *pOut << xyz << "  " << xyzs[i] << "  " <<
            dif2 << "  " << isOk_ << "\n\n";
      }

      //if( pOut && isVerbose ) *pOut << "\n";

      if( pOut ) *pOut << "constants : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   // queries consistency
   {
      bool isOk_ = true;

      ColorSpace ct1;

      const Vector3f rgb1( ct1.transXyzToRgb_( Vector3f::ONE() * 3.25f ) );
      Vector3f rgb2( Vector3f::ONE() * 3.25f );
      ct1.transXyzToRgb( rgb2 );

      isOk_ &= ::fabsf((rgb1 - rgb2).largest()) < 1e-5f;
      if( pOut && isVerbose ) *pOut << rgb1 << "  " << rgb2 << "  " <<
         (rgb1 - rgb2) << "  " << isOk_ << "\n";

      const Vector3f xyz1( ct1.transRgbToXyz_( Vector3f::ONE() * 3.25f ) );
      Vector3f xyz2( Vector3f::ONE() * 3.25f );
      ct1.transRgbToXyz( xyz2 );
      const float y3 = ct1.getRgbLuminance( Vector3f::ONE() * 3.25f );

      isOk_ &= (::fabsf((xyz1 - xyz2).largest()) < 1e-5f) &
         (::fabsf(xyz1.getY() - y3) < 1e-5f);
      if( pOut && isVerbose ) *pOut << xyz1 << "  " << xyz2 << "  " <<
         (xyz1 - xyz2) << "  " << y3 << "  " << (xyz1.getY() - y3) << "  " <<
         isOk_ << "\n";

      if( pOut && isVerbose ) *pOut << "\n";

      if( pOut ) *pOut << "consistency : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
      " completed " << "\n\n\n";

   if( pOut ) pOut->flush();


   return isOk;
}


}//namespace


#endif//TESTING
