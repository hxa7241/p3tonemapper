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


#include <time.h>   /// DEBUG ///
#include <iostream>   /// DEBUG ///

#include <float.h>
#include <math.h>
#include <string.h>
#include <exception>

#include "Clamps.hpp"

#include "Vector3f.hpp"
#include "ColorConstants.hpp"

#include "ColorSpace.hpp"
#include "ImageRgbFloat.hpp"
#include "ImageRgbInt.hpp"

#include "Foveal.hpp"
#include "Veil.hpp"
#include "ColorAdjustment.hpp"
#include "AcuityFilter.hpp"
#include "ToneAdjustment.hpp"

#include "PerceptualMap.hpp"   // own header is included last


using namespace p3tonemapper_tonemap;




/// standard object services ---------------------------------------------------
PerceptualMap::PerceptualMap()
{
   PerceptualMap::setInputColorSpace( 0, 0 );
   PerceptualMap::setInputLuminanceScale( 0 );
   PerceptualMap::setInputViewAngle( 0.0f );
   PerceptualMap::setMappingFeatures( 0 );
   PerceptualMap::setOutputLuminanceRange( 0 );
   PerceptualMap::setOutputGamma( 0.0f );
}


PerceptualMap::PerceptualMap
(
   const float* pInChromaticities6,
   const float* pInWhitePoint2,
   const float* pInScalingAndOffset2,
   const float  inViewAngleHorizontal,
   const dword  mappingFlags,
   const float* pOutLuminanceRange2,
   const float  outGamma
)
{
   PerceptualMap::setInputColorSpace( pInChromaticities6, pInWhitePoint2 );
   PerceptualMap::setInputLuminanceScale( pInScalingAndOffset2 );
   PerceptualMap::setInputViewAngle( inViewAngleHorizontal );
   PerceptualMap::setMappingFeatures( mappingFlags );
   PerceptualMap::setOutputLuminanceRange( pOutLuminanceRange2 );
   PerceptualMap::setOutputGamma( outGamma );
}


PerceptualMap::~PerceptualMap()
{
}


PerceptualMap::PerceptualMap
(
   const PerceptualMap& other
)
{
   PerceptualMap::operator=( other );
}


PerceptualMap& PerceptualMap::operator=
(
   const PerceptualMap& other
)
{
   if( &other != this )
   {
      PerceptualMap::setInputColorSpace(
         other.inputChromaticities_m, other.inputWhitePoint_m );

      inputLuminanceScaling_m = other.inputLuminanceScaling_m;
      inputLuminanceOffset_m  = other.inputLuminanceOffset_m;

      inputViewAngleHorizontal_m = other.inputViewAngleHorizontal_m;

      mappingFlags_m = other.mappingFlags_m;

      outputBlackLuminance_m = other.outputBlackLuminance_m;
      outputWhiteLuminance_m = other.outputWhiteLuminance_m;

      outputGamma_m = other.outputGamma_m;
   }

   return *this;
}




/// commands -------------------------------------------------------------------
void PerceptualMap::setInputColorSpace
(
   const float* pInChromaticities6,
   const float* pInWhitePoint2
)
{
   if( 0 == pInChromaticities6 )
   {
      pInChromaticities6 =
         hxa7241_graphics::ColorConstants::getSrgbChromaticities();
   }
   for( int i = 6;  i-- > 0; )
   {
      inputChromaticities_m[i] = pInChromaticities6[i];
   }

   if( 0 == pInWhitePoint2 )
   {
      pInWhitePoint2 =
         hxa7241_graphics::ColorConstants::getSrgbWhitePoint();
   }
   for( int i = 2;  i-- > 0; )
   {
      inputWhitePoint_m[i] = pInWhitePoint2[i];
   }
}


void PerceptualMap::setInputLuminanceScale
(
   const float* pInScalingAndOffset2
)
{
   if( 0 == pInScalingAndOffset2 )
   {
      static const float IN_SCALE[2] = { 1.0f, 0.0f };
      pInScalingAndOffset2 = IN_SCALE;
   }

   inputLuminanceScaling_m = pInScalingAndOffset2[0];
   inputLuminanceOffset_m  = pInScalingAndOffset2[1];
}


void PerceptualMap::setInputViewAngle
(
   float inViewAngleHorizontal
)
{
   if( 0.0f >= inViewAngleHorizontal )
   {
      inViewAngleHorizontal = 65.0f;
   }

   inputViewAngleHorizontal_m = inViewAngleHorizontal;
}


void PerceptualMap::setMappingFeatures
(
   const dword mappingFlags
)
{
   mappingFlags_m = mappingFlags;
}


void PerceptualMap::setOutputLuminanceRange
(
   const float* pOutLuminanceRange2
)
{
   if( 0 == pOutLuminanceRange2 )
   {
      pOutLuminanceRange2 =
         hxa7241_graphics::ColorConstants::getTftLuminanceRange();
   }

   // clamp black between 0 and 1000
   outputBlackLuminance_m = hxa7241_general::clamp(
      pOutLuminanceRange2[0], 0.0f, 1000.0f );

   // clamp white between black+10 and 100000
   outputWhiteLuminance_m = hxa7241_general::clamp(
      pOutLuminanceRange2[1], outputBlackLuminance_m + 10.0f, 100000.0f );
}


void PerceptualMap::setOutputGamma
(
   float outGamma
)
{
   if( 0.0f == outGamma )
   {
      outGamma = hxa7241_graphics::ColorConstants::get709Gamma();
   }

   outputGamma_m = outGamma;
}




/// queries --------------------------------------------------------------------
void PerceptualMap::getOptions
(
   float*const pInChromaticities6,
   float*const pInWhitePoint2,
   float*const pInScalingAndOffset2,
   float*const pInViewAngleHorizontal,
   dword*const pMappingFlags,
   float*const pOutLuminanceRange2,
   float*const pOutGamma
) const
{
   // copy colorspace
   if( 0 != pInChromaticities6 )
   {
      for( int i = 6;  i-- > 0; )
      {
         pInChromaticities6[i] = inputChromaticities_m[i];
      }
   }
   if( 0 != pInWhitePoint2 )
   {
      for( int i = 2;  i-- > 0; )
      {
         pInWhitePoint2[i] = inputWhitePoint_m[i];
      }
   }

   // copy luminance scale
   if( 0 != pInScalingAndOffset2 )
   {
      pInScalingAndOffset2[0] = inputLuminanceScaling_m;
      pInScalingAndOffset2[1] = inputLuminanceOffset_m;
   }

   // copy view angle
   if( 0 != pInViewAngleHorizontal )
   {
      *pInViewAngleHorizontal = inputViewAngleHorizontal_m;
   }

   // copy mapping features
   if( 0 != pMappingFlags )
   {
      *pMappingFlags = mappingFlags_m;
   }

   // copy output luminance range
   if( 0 != pOutLuminanceRange2 )
   {
      pOutLuminanceRange2[0] = outputBlackLuminance_m;
      pOutLuminanceRange2[1] = outputWhiteLuminance_m;
   }

   // copy output gamma
   if( 0 != pOutGamma )
   {
      *pOutGamma = outputGamma_m;
   }
}


bool PerceptualMap::map
(
   const dword  width,
   const dword  height,
   const dword  ,//inPixelsType,
   void*        pInPixels,
   const dword  outPixelsType,
   void*        pOutPixels,
   int*         pAsyncProgress,
   char*        pMessage128
) const
{
   bool isOk = false;
   if( pMessage128 )
   {
      pMessage128[ 0 ] = 0;
   }

/// DEBUG ///
clock_t times[20];
int     ti = 0;
times[ti] = ::clock();
std::cout << "start time = " << times[ti++] << "\n";   /// DEBUG ///

#ifndef __STRICT_ANSI__
   // set fp control word: rounding mode near, no exceptions
   // (This may not be ANSI. If it won't work, remove it (and the other one
   // below).)
   const unsigned int fpControlWord =
      ::_controlfp( _MCW_EM | _RC_NEAR, _MCW_EM | _MCW_RC );
#endif //__STRICT_ANSI__

   try
   {
      using p3tonemapper_image::ColorSpace;
      using p3tonemapper_image::ImageRgbFloat;

      // make color transform for original image
      const ColorSpace colorSpace(
         inputChromaticities_m, inputWhitePoint_m );

times[ti] = ::clock();   /// DEBUG ///
std::cout << "calibrate start = " << times[ti++] << "\n";   /// DEBUG ///

      // make wrapper for original image
      ImageRgbFloat original( width, height, static_cast<float*>(pInPixels),
         false, colorSpace );
      if( (1.0f != inputLuminanceScaling_m) | (0.0f != inputLuminanceOffset_m) )
      {
         using hxa7241_graphics::Vector3f;

         // scale and offset image
         const Vector3f offset( Vector3f::ONE() * inputLuminanceOffset_m );
         for( dword op = original.getLength();  op-- > 0; )
         {
            original.set( op,
               (original.get( op ) *= inputLuminanceScaling_m) += offset );
         }
      }

times[ti] = ::clock();   /// DEBUG ///
std::cout << "foveal start = " << times[ti++] << "\n";   /// DEBUG ///

      // make foveal image
      Foveal foveal( original, inputViewAngleHorizontal_m );

      // apply supplementary human limitations
      const bool isHumanContrast = (mappingFlags_m & HUMAN) != 0;
      if( isHumanContrast )
      {
times[ti] = ::clock();   /// DEBUG ///
std::cout << "glare start = " << times[ti++] << "\n";   /// DEBUG ///

         // glare
         if( mappingFlags_m & (GLARE & ~CONTRAST) )
         {
            Veil veil( foveal );

times[ti] = ::clock();   /// DEBUG ///
std::cout << "veil mix 1 start= " << times[ti++] << "\n";   /// DEBUG ///

            veil.mixInto( foveal );
times[ti] = ::clock();   /// DEBUG ///
std::cout << "veil mix 2 start = " << times[ti++] << "\n";   /// DEBUG ///

            veil.mixInto( original );
         }

         // color sensitivity
         if( mappingFlags_m & (COLOR & ~CONTRAST) )
         {
times[ti] = ::clock();   /// DEBUG ///
std::cout << "color start = " << times[ti++] << "\n";   /// DEBUG ///

            ColorAdjustment colorAdjustment(
               foveal.getColorSpace(), original );

            ImageRgbFloat::visitBilinear( foveal, colorAdjustment,
               original.getWidth(), original.getHeight() );
         }

         // spatial acuity
         if( mappingFlags_m & (ACUITY & ~CONTRAST) )
         {
times[ti] = ::clock();   /// DEBUG ///
std::cout << "acuity start = " << times[ti++] << "\n";   /// DEBUG ///

            // copy original to temp
            ImageRgbFloat intermediate( original );

            AcuityFilter acuityFilter(
               foveal.getColorSpace(), intermediate, original );

            ImageRgbFloat::visitBilinear( foveal, acuityFilter,
               original.getWidth(), original.getHeight() );
         }
      }

times[ti] = ::clock();   /// DEBUG ///
std::cout << "general map start = " << times[ti++] << "\n";   /// DEBUG ///

      // make wrapper for output image
      ImageRgbInt outImage( width, height, RGB_WORD == outPixelsType,
         false, pOutPixels );
      outImage.setGamma( outputGamma_m );

      // do main tone mapping
      ToneAdjustment toneAdjustment( foveal,
         outputBlackLuminance_m, outputWhiteLuminance_m,
         isHumanContrast );
      toneAdjustment.map( original, outImage );

      isOk = true;
   }
   catch( const std::exception& exception )
   {
      if( pMessage128 )
      {
         ::strncpy( pMessage128, exception.what(), 127 );
         pMessage128[ 127 ] = 0;
      }
   }
   catch( const char*const exceptionString )
   {
      if( pMessage128 )
      {
         ::strncpy( pMessage128, exceptionString, 127 );
         pMessage128[ 127 ] = 0;
      }
   }
   catch( ... )
   {
      if( pMessage128 )
      {
         ::strncpy( pMessage128, "unannotated exception", 127 );
         pMessage128[ 127 ] = 0;
      }
   }

#ifndef __STRICT_ANSI__
   // restore fp control word
   ::_controlfp( fpControlWord, 0xFFFFFFFFu );
#endif //__STRICT_ANSI__

times[ti] = ::clock();   /// DEBUG ///
std::cout << "end time = " << times[ti++] << "\n";   /// DEBUG ///
/// DEBUG ///
std::cout << "\n";   /// DEBUG ///
std::cout << 0 << "\n";
clock_t duration = times[ti - 1] - times[0];
for( int g = 1;  g < ti;  ++g )
{
   std::cout << (((times[g] - times[0]) * 100.0f) / duration) << "  " <<
      (((times[g] - times[g - 1]) * 100.0f) / duration) << "  " <<
      (times[g] - times[g - 1]) << "\n";
}

   return isOk;
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <ostream>


namespace p3tonemapper_tonemap
{
   using namespace hxa7241;


bool test_PerceptualMap
(
   std::ostream* pOut,
   const bool    ,//verbose,
   const dword   //seed
)
{
   bool isOk = true;

   if( pOut ) *pOut << "[ test_PerceptualMap ]\n\n";


   if( pOut ) *pOut << "!!! not implemented yet\n\n";
   //isOk = false;


   if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
      " completed " << "\n\n\n";

   if( pOut ) pOut->flush();


   return isOk;
}


}//namespace


#endif//TESTING
