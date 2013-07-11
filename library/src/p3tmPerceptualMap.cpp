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


#include <string.h>

#include "PerceptualMap.hpp"

#include "p3tmPerceptualMap-v13.h"


using p3tonemapper_tonemap::PerceptualMap;




/// constants ------------------------------------------------------------------
namespace
{
   const char MESSAGE_CREATE_FAILED[] = "mapper creation failed";
}




/// version meta-interface =====================================================

int p3tmIsVersionSupported
(
   const int versionId
)
{
   return (p3tm11_VERSION == versionId) | (p3tm13_VERSION == versionId) ?
      p3tm_SUPPORTED_FULLY : p3tm_SUPPORTED_NOT;
}








/// wrapper functions ==========================================================

int p3tmMapFBSimple
(
   int    i_mappingFlags,
   int    i_width,
   int    i_height,
   float* i_pInPixels,
   char*  o_pOutPixels,
   int*   o_pAsyncProgress
)
{
   // delegate with default options (except mapping flags)
   return p3tmMapFBFull( 0, 0, 0, 0.0f, i_mappingFlags, 0, 0.0f,
      i_width, i_height, i_pInPixels, o_pOutPixels, o_pAsyncProgress, 0 );
}


int p3tmMapFBFull
(
   const float* i_pInChromaticities6,
   const float* i_pInWhitePoint2,
   const float* i_pInScalingAndOffset2,
   float        i_inViewAngleHorizontalDegrees,
   int          i_mappingFlags,
   const float* i_pOutLuminanceRange2,
   float        i_outGamma,
   int          i_width,
   int          i_height,
   float*       i_pInPixels,
   char*        o_pOutPixels,
   int*         o_pAsyncProgress,
   char*        o_pMessage128
)
{
   // create mapper
   void* pPm = p3tmCreatePerceptualMap(
      i_pInChromaticities6,
      i_pInWhitePoint2,
      i_pInScalingAndOffset2,
      i_inViewAngleHorizontalDegrees,
      i_mappingFlags,
      i_pOutLuminanceRange2,
      i_outGamma );

   int ok = 0;
   // check for creation failure
   if( pPm )
   {
      // map
      ok = p3tmMapFB( pPm, i_width, i_height, i_pInPixels,
         o_pOutPixels, o_pAsyncProgress, o_pMessage128 );

      // free mapper (ignore failure)
      p3tmFreePerceptualMap( pPm );
   }
   else
   {
      // fill exception message
      if( o_pMessage128 )
      {
         ::strncpy( o_pMessage128, MESSAGE_CREATE_FAILED, 127 );
         o_pMessage128[ 127 ] = 0;
      }
   }

   return ok;
}


int p3tmMapFB
(
   const void* i_pPm,
   int         i_width,
   int         i_height,
   float*      i_pInPixels,
   char*       o_pOutPixels,
   int*        o_pAsyncProgress,
   char*       o_pMessage128
)
{
   // insert type flags and delegate
   return p3tmMap2( i_pPm, i_width, i_height,
      p3tm11_RGB_FLOAT, i_pInPixels, p3tm11_RGB_BYTE, o_pOutPixels,
      o_pAsyncProgress, o_pMessage128 );
}




/// supplementary method =======================================================

int p3tmMap2
(
   const void* pPm,
   int         width,
   int         height,
   int         inPixelsType,
   void*       pInPixels,
   int         outPixelsType,
   void*       pOutPixels,
   int*        pAsyncProgress,
   char*       pMessage128
)
{
   return static_cast<const PerceptualMap*>( pPm )->map(
      width,
      height,
      inPixelsType,
      pInPixels,
      outPixelsType,
      pOutPixels,
      pAsyncProgress,
      pMessage128 ) ? 1 : 0;
}








/// object interface ===========================================================

/// basic object services ------------------------------------------------------
void* p3tmCreatePerceptualMapDefault()
{
   void* pPm = 0;

   try
   {
      pPm = new PerceptualMap();
   }
   catch( ... )
   {
      // ummm, this shouldnt be needed...
      pPm = 0;
   }

   return pPm;
}


void* p3tmCreatePerceptualMap
(
   const float* pInChromaticities6,
   const float* pInWhitePoint2,
   const float* pInScalingAndOffset2,
   float        inViewAngleHorizontalDegrees,
   int          mappingFlags,
   const float* pOutLuminanceRange2,
   float        outGamma
)
{
   void* pPm = 0;

   try
   {
      pPm = new PerceptualMap(
         pInChromaticities6,
         pInWhitePoint2,
         pInScalingAndOffset2,
         inViewAngleHorizontalDegrees,
         mappingFlags,
         pOutLuminanceRange2,
         outGamma
      );
   }
   catch( ... )
   {
      // ummm, this shouldnt be needed...
      pPm = 0;
   }

   return pPm;
}


int p3tmFreePerceptualMap
(
   void* pObject
)
{
   bool isOk = true;

   try
   {
      delete static_cast<PerceptualMap*>( pObject );
   }
   catch( ... )
   {
      isOk = false;
   }

   return isOk ? 1 : 0;
}


void* p3tmCreatePerceptualMapCopy
(
   const void* pOther
)
{
   void* pPm = 0;

   try
   {
      pPm = new PerceptualMap( *static_cast<const PerceptualMap*>( pOther ) );
   }
   catch( ... )
   {
      // ummm, this shouldnt be needed...
      pPm = 0;
   }

   return pPm;
}


void p3tmAssignPerceptualMap
(
   const void* pSource,
   void*       pTarget
)
{
   *static_cast<PerceptualMap*>( pTarget ) =
      *static_cast<const PerceptualMap*>( pSource );
}




/// commands -------------------------------------------------------------------
void p3tmSetInputColorSpace
(
   void*        pPm,
   const float* pChromaticities6,
   const float* pWhitePoint2
)
{
   static_cast<PerceptualMap*>( pPm )->setInputColorSpace(
      pChromaticities6,
      pWhitePoint2 );
}


void p3tmSetInputLuminanceScale
(
   void*        pPm,
   const float* pScalingAndOffset2
)
{
   static_cast<PerceptualMap*>( pPm )->setInputLuminanceScale(
      pScalingAndOffset2 );
}


void p3tmSetInputViewAngle
(
   void* pPm,
   float viewAngleHorizDegrees
)
{
   static_cast<PerceptualMap*>( pPm )->setInputViewAngle(
      viewAngleHorizDegrees );
}


void p3tmSetMappingFeatures
(
   void* pPm,
   int   mappingFlags
)
{
   static_cast<PerceptualMap*>( pPm )->setMappingFeatures(
      mappingFlags );
}


void p3tmSetOutputLuminanceRange
(
   void*        pPm,
   const float* pLuminanceRange2
)
{
   static_cast<PerceptualMap*>( pPm )->setOutputLuminanceRange(
      pLuminanceRange2 );
}


void p3tmSetOutputGamma
(
   void* pPm,
   float outGamma
)
{
   static_cast<PerceptualMap*>( pPm )->setOutputGamma(
      outGamma );
}




/// queries --------------------------------------------------------------------
void p3tmGetOptions
(
   const void* pPm,
   float*      pInChromaticities6,
   float*      pInWhitePoint2,
   float*      pInScalingAndOffset2,
   float*      pInViewAngleHorizontal,
   int*        pMappingFlags,
   float*      pOutLuminanceRange2,
   float*      pOutGamma
)
{
   static_cast<const PerceptualMap*>( pPm )->getOptions(
      pInChromaticities6,
      pInWhitePoint2,
      pInScalingAndOffset2,
      pInViewAngleHorizontal,
      pMappingFlags,
      pOutLuminanceRange2,
      pOutGamma );
}


int p3tmMap
(
   const void* pPm,
   int         width,
   int         height,
   int         inPixelsType,
   void*       pInPixels,
   int         outPixelsType,
   void*       pOutPixels,
   char*       pMessage128
)
{
   return static_cast<const PerceptualMap*>( pPm )->map(
      width,
      height,
      inPixelsType,
      pInPixels,
      outPixelsType,
      pOutPixels,
      0,
      pMessage128 ) ? 1 : 0;
}








/// test =======================================================================

#ifndef TESTING


int p3tmTestUnits
(
   int       ,
   const int ,
   const int ,
   const int
)
{
   return 0;
}


#else


#include <iostream>


using namespace hxa7241;


/// unit test declarations
namespace hxa7241_general
{
   bool test_FpToInt        ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_Array          ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_Sheet          ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_Interval       ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_Histogram      ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_SamplesRegular1( std::ostream* pOut, bool isVerbose, dword seed );
}

namespace p3tonemapper_image
{
   bool test_ColorSpace   ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_ImageRgbInt  ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_ImageRgbFloat( std::ostream* pOut, bool isVerbose, dword seed );
}

namespace p3tonemapper_tonemap
{
   bool test_Foveal         ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_Veil           ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_ColorAdjustment( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_AcuityFilter   ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_ToneAdjustment ( std::ostream* pOut, bool isVerbose, dword seed );
   bool test_PerceptualMap  ( std::ostream* pOut, bool isVerbose, dword seed );
}



/// unit test caller
static bool (*TESTERS[])(std::ostream*, bool, dword) =
{
   &hxa7241_general::test_FpToInt                //  1
,  &hxa7241_general::test_Array                  //  2
,  &hxa7241_general::test_Sheet                  //  3
,  &hxa7241_general::test_Interval               //  4
,  &hxa7241_general::test_Histogram              //  5
,  &hxa7241_general::test_SamplesRegular1        //  6

,  &p3tonemapper_image::test_ColorSpace          //  7
,  &p3tonemapper_image::test_ImageRgbInt         //  8
,  &p3tonemapper_image::test_ImageRgbFloat       //  9

,  &p3tonemapper_tonemap::test_Foveal            // 10
,  &p3tonemapper_tonemap::test_Veil              // 11
,  &p3tonemapper_tonemap::test_ColorAdjustment   // 12
,  &p3tonemapper_tonemap::test_AcuityFilter      // 13
,  &p3tonemapper_tonemap::test_ToneAdjustment    // 14
,  &p3tonemapper_tonemap::test_PerceptualMap     // 15
};


int p3tmTestUnits
(
   int       whichTest,
   const int isOutput,
   const int isVerbose,
   const int seed
)
{
   bool isOk = true;

   if( isOutput ) std::cout << "\n\n";

   const dword noOfTests = sizeof(TESTERS)/sizeof(TESTERS[0]);
   if( 0 >= whichTest )
   {
      for( dword i = 0;  i < noOfTests;  ++i )
      {
         isOk &= (TESTERS[i])(
            isOutput ? &std::cout : 0, 0 != isVerbose, seed );
      }
   }
   else
   {
      if( whichTest > noOfTests )
      {
         whichTest = noOfTests;
      }
      isOk &= (TESTERS[whichTest - 1])(
         isOutput ? &std::cout : 0, 0 != isVerbose, seed );
   }

   if( isOutput ) std::cout <<
      (isOk ? "--- successfully" : "*** failurefully") << " completed " <<
      ((0 >= whichTest) ? "all lib unit tests" : "one lib unit test") << "\n\n";

   return isOk;
}


#endif//TESTING
