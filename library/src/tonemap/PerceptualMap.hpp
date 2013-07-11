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


#ifndef PerceptualMap_h
#define PerceptualMap_h


#include "p3tmPerceptualMap-v13.h"




#include "p3tonemapper_tonemap.hpp"
namespace p3tonemapper_tonemap
{


/**
 * A perceptual tonemapping operator, to convert HDR RGB images into int RGB
 * images.<br/><br/>
 *
 * Offers two kinds of tonemapping: ideal viewer, and human viewer.
 * For the ideal viewer an optimal tonemapping is made. For the human viewer the
 * optimal tonemapping has clarity reduced to simulate human perceptual limits.
 * <br/><br/>
 *
 * @implementation
 * derived from the paper:
 * <cite>'A Visibility Matching Tone Reproduction Operator for High Dynamic
 * Range Scenes'
 * Ward Larson, Rushmeier, Piatko;
 * University Of California 1997.</cite>
 */
class PerceptualMap
{
public:
   /**
    * For use with constructor mappingFlags parameter, and setMappingFeatures.
    *
    * IDEAL can be combined with the other human effects. GLARE, COLOR and
    * ACUITY imply CONTRAST. HUMAN is all human effects.
    */
   enum EMappingOptions
   {
      IDEAL    = p3tm11_IDEAL,
      CONTRAST = p3tm11_CONTRAST,
      GLARE    = p3tm11_GLARE,
      COLOR    = p3tm11_COLOR,
      ACUITY   = p3tm11_ACUITY,
      HUMAN    = p3tm11_HUMAN
   };


/// standard object services ---------------------------------------------------
public:
   /**
    * Create a default mapper.<br/><br/>
    *
    * Defaults to: ITU-R BT.709 / sRGB input color space, cd/m^2 luminance,
    * 65 degrees, ideal viewer, TFT display range, ITU-R BT.709 gamma.
    */
            PerceptualMap();
   /**
    * Create a specified mapper.<br/><br/>
    *
    * Give zeros to set to defaults.<br/><br/>
    *
    * @pInChromaticities6     array of six floats { rx, ry, gx, gy, bx, by }
    *                         each value is >= 0 and <= 1
    *                         (default: ITU-R BT.709 / sRGB)
    * @pInWhitePoint2         array of two floats, each value is > 0 and < 1
    *                         (default: ITU-R BT.709 / sRGB)
    * @pInScalingAndOffset2   a transform to calibrate input pixels into cd/m^2.
    *                         array of two floats
    *                         (default: 1 and 0)
    * @inViewAngleHorizontal  horizontal angle of view frustrum in degrees
    *                         (default: 65)
    * @mappingFlags           or-able bit options for the mapping, from
    *                         EMappingOptions.
    *                         (default: p3tm11_IDEAL)
    * @pOutLuminanceRange2    min and max display luminance (cd/m^2).
    *                         array of two floats
    *                         (default: 2 and 150 (TFT))
    * @outGamma               power transform to apply to output pixels
    *                         (default: ITU-R BT.709: 0.45)
    */
            PerceptualMap( const float* pInChromaticities6,
                           const float* pInWhitePoint2,
                           const float* pInScalingAndOffset2,
                           float        inViewAngleHorizontal,
                           dword        mappingFlags,
                           const float* pOutLuminanceRange2,
                           float        outGamma );

   virtual ~PerceptualMap();
            PerceptualMap( const PerceptualMap& );
   PerceptualMap& operator=( const PerceptualMap& );


/// commands -------------------------------------------------------------------
   /**
    * Set the input image chromaticities and whitepoint.<br/><br/>
    *
    * Give zeros for defaults (ITU-R BT.709 / sRGB).<br/><br/>
    *
    * @pInChromaticities6  array of six floats { rx, ry, gx, gy, bx, by }
    *                      each value is >= 0 and <= 1
    * @pInWhitePoint2      array of two floats, each value is > 0 and < 1
    */
   virtual void  setInputColorSpace     ( const float* pInChromaticities6,
                                          const float* pInWhitePoint2 );
   /**
    * Set the input pixels transform.<br/><br/>
    *
    * @pInScalingAndOffset2  a transform to calibrate input pixels into cd/m^2.
    *                        array of two floats. Give zero to set to default
    *                        (1 and 0).
    */
   virtual void  setInputLuminanceScale ( const float* pInScalingAndOffset2 );
   /**
    * Set the input image view angle.<br/><br/>
    *
    * @inViewAngleHorizontal  horizontal angle of view frustrum in degrees.
    *                         Give zero to set to default (65).
    */
   virtual void  setInputViewAngle      ( float        inViewAngleHorizontal );
   /**
    * Set the mapping options.<br/><br/>
    *
    * @mappingFlags  or-able bit options for the mapping, from EMappingOptions.
    *                Give zero to set to default (p3tm11_IDEAL).
    */
   virtual void  setMappingFeatures     ( dword        mappingFlags );
   /**
    * Set the output display range.<br/><br/>
    *
    * @pOutLuminanceRange2  min and max display luminance (cd/m^2).
    *                       array of two floats. Give zero to set to default
    *                       (2 and 150 (TFT)).
    */
   virtual void  setOutputLuminanceRange( const float* pOutLuminanceRange2 );
   /**
    * Set the output gamma power transform.<br/><br/>
    *
    * @outGamma  power transform to apply to output pixels. Give zero to set to
    *            default (ITU-R BT.709: 0.45).
    */
   virtual void  setOutputGamma         ( float        outGamma );


/// queries --------------------------------------------------------------------
   /**
    * Get options.<br/><br/>
    *
    * Give zeros for values not wanted.<br/><br/>
    *
    * @pInChromaticities6      array of six floats { rx, ry, gx, gy, bx, by }
    * @pInWhitePoint2          array of two floats
    * @pInScalingAndOffset2    a transform to calibrate input pixels to cd/m^2.
    *                          array of two floats
    * @pInViewAngleHorizontal  horizontal angle of view frustrum in degrees
    * @pMappingFlags           or-able bit options for the mapping, from
    *                          EMappingOptions.
    * @pOutLuminanceRange2     min and max display luminance (cd/m^2).
    *                          array of two floats
    * @pOutGamma               power transform applied to output pixels
    */
   virtual void  getOptions( float* pInChromaticities6,
                             float* pInWhitePoint2,
                             float* pInScalingAndOffset2,
                             float* pInViewAngleHorizontal,
                             dword* pMappingFlags,
                             float* pOutLuminanceRange2,
                             float* pOutGamma )                           const;

   /**
    * For use with map inPixelsType parameter.
    *
    * @RGB_FLOAT   R then G then B, each is a float
    */
   enum EInPixelOptions
   {
      RGB_FLOAT = p3tm11_RGB_FLOAT
   };

   /**
    * For use with map outPixelsType parameter.
    *
    * @RGB_BYTE   R then G then B, each is a byte
    * @RGB_WORD   R then G then B, each is a word
    */
   enum EOutPixelOptions
   {
      RGB_BYTE = p3tm11_RGB_BYTE,
      RGB_WORD = p3tm11_RGB_WORD
   };

   /**
    * Map an image.<br/><br/>
    *
    * @width           width of input and output images
    * @height          height of input and output images
    * @inPixelsType    input pixels type, a p3tmEInPixelOptions value
    * @pInPixels       array of input RGB pixels
    * @outPixelsType   output pixels type, a p3tmEOutPixelOptions value
    * @pOutPixels      array of output RGB pixels
    * @pAsyncProgress  percentage progress feedback to be read by another thread
    * @pMessage128     string for exception message 128 chars long
    *
    * @return  is successful
    */
   virtual bool  map( dword  width,
                      dword  height,
                      dword  inPixelsType,
                      void*  pInPixels,
                      dword  outPixelsType,
                      void*  pOutPixels,
                      int*   pAsyncProgress,
                      char*  pMessage128 )                                const;


/// fields ---------------------------------------------------------------------
private:
   // input color space
   float inputChromaticities_m[6];
   float inputWhitePoint_m[2];

   // input luminance transform
   float inputLuminanceScaling_m;
   float inputLuminanceOffset_m;

   // input view angle
   float inputViewAngleHorizontal_m;

   // mapping options
   dword mappingFlags_m;

   // output luminance range
   float outputBlackLuminance_m;
   float outputWhiteLuminance_m;

   // output gamma
   float outputGamma_m;
};


}//namespace




#endif//PerceptualMap_h
