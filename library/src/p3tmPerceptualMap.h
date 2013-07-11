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


/**
 * Interface for the P3 Tonemapper perceptual tonemapping component. Converts
 * HDR RGB images into integer RGB images.<br/><br/>
 *
 * <cite>http://www.hxa7241.org/tonemapper/</cite>
 *
 *
 * The library has a C interface, defined in p3tmPerceptualMap-v__.h and
 * p3tmPerceptualMap.h . The former, main, part should be stable across
 * versions. The latter specifies constants that will change with versions.
 * There are no other dependencies.<br/><br/>
 *
 * To build a client: include p3tmPerceptualMap-v__.h and link with the
 * p3tonemapper.lib|.a import library (Windows) or the libp3tonemapper.so
 * library (Linux), or access purely dynamically.<br/><br/>
 *
 *
 * There are three interface sections: meta-versioning, wrapper, object.
 * <br/><br/>
 *
 * Versioning meta interface:<br/>
 * For checking a dynamically linked library supports the interfaces here.
 * <br/><br/>
 *
 * Wrapper interface:<br/>
 * This provides simpler, more type-safe, alternatives to the object interface.
 * You can tonemap an image with a single function call, providing as many
 * options as convenient (and have pixel types checked (partially)).<br/><br/>
 *
 * Object interface:<br/>
 * This is the underlying, most generally defined, interface.
 * It follows an object pattern, in three parts: construction and destruction,
 * commands to set options, and queries to operate on data. This form allows the
 * implementation to cache things, which might have performance advantages in
 * future versions. The use of pixel type-codes and void*s allows any format to
 * be supported in future versions.<br/><br/>
 */




#ifndef p3tmPerceptualMap_h
#define p3tmPerceptualMap_h


#ifdef __cplusplus
extern "C" {
#endif




/*= version meta-interface ===================================================*/

/**
 * Return values for p3tmIsVersionSupported().
 */
enum p3tmESupported
{
   p3tm_SUPPORTED_NOT   = 0,
   p3tm_SUPPORTED_FULLY = 2
};


/**
 * Get whether this interface is supported by the implementation.<br/><br/>
 *
 * (For checking a dynamically linked library implementation.)
 *
 * @versionId  version ID value from the options/constants header.
 * @return     a p3tmESupported value
 */
int p3tmIsVersionSupported
(
   int versionId
);








/*= wrapper functions (new for version 1.3) ==================================*/

/**
 * Map an image (with single function call, and simple options).<br/><br/>
 *
 * @i_mappingFlags   or-able bit options for the mapping, from the
 *                   options/constants header.
 *                   (give 0 for default: ideal)
 * @i_width          width of input and output images
 * @i_height         height of input and output images
 * @i_inPixels       array of input RGB pixels, floats, channels in R,G,B order
 * @o_outPixels      array of output RGB pixels, bytes, channels in R,G,B order
 * @o_asyncProgress  percentage progress feedback to be read by another thread
 *
 * @return  1 means succeeded, 0 means failed
 */
int p3tmMapFBSimple
(
   int    i_mappingFlags,
   int    i_width,
   int    i_height,
   float* i_inPixels,
   char*  o_outPixels,
   int*   o_asyncProgress
);


/**
 * Map an image (with single function call, and all options).<br/><br/>
 *
 * @i_inChromaticities6      array of six floats { rx, ry, gx, gy, bx, by }
 *                           each value is >= 0 and <= 1
 *                           (give 0 for default: ITU-R BT.709 / sRGB)
 * @i_inWhitePoint2          array of two floats, each value is > 0 and < 1
 *                           (give 0 for default: ITU-R BT.709 / sRGB)
 * @i_inScalingAndOffset2    a transform to calibrate input pixels into cd/m^2.
 *                           array of two floats
 *                           (give 0 for default: 1 and 0)
 * @i_inViewAngleHorizontal  horizontal angle of view frustrum in degrees
 *                           (give 0 for default: 65)
 * @i_mappingFlags           or-able bit options for the mapping, from the
 *                           options/constants header.
 *                           (give 0 for default: ideal)
 * @i_outLuminanceRange2     min and max display luminance (cd/m^2).
 *                           array of two floats
 *                           (give 0 for default: 2 and 150 (TFT))
 * @i_outGamma               power transform to apply to output pixels
 *                           (give 0 for default: ITU-R BT.709: 0.45)
 * @i_width                  width of input and output images
 * @i_height                 height of input and output images
 * @i_inPixels               array of input RGB pixels, floats, channels in
 *                           R,G,B order
 * @o_outPixels              array of output RGB pixels, bytes, channels in
 *                           R,G,B order
 * @o_asyncProgress          percentage progress feedback to be read by another
 *                           thread
 * @o_message128             string for exception message 128 chars long, (or 0)
 *
 * @return  1 means succeeded, 0 means failed
 */
int p3tmMapFBFull
(
   const float* i_inChromaticities6,
   const float* i_inWhitePoint2,
   const float* i_inScalingAndOffset2,
   float        i_inViewAngleHorizontal,
   int          i_mappingFlags,
   const float* i_outLuminanceRange2,
   float        i_outGamma,
   int          i_width,
   int          i_height,
   float*       i_inPixels,
   char*        o_outPixels,
   int*         o_asyncProgress,
   char*        o_message128
);


/**
 * Map an image (typed for float and byte images).<br/><br/>
 *
 * @i_perceptualMap  object from one of the p3tmCreate___ functions
 * @i_width          width of input and output images
 * @i_height         height of input and output images
 * @i_inPixels       array of input RGB pixels, floats, channels in R,G,B order
 * @o_outPixels      array of output RGB pixels, bytes, channels in R,G,B order
 * @o_asyncProgress  percentage progress feedback to be read by another thread
 * @o_message128     string for exception message 128 chars long, or (or 0)
 *
 * @return  1 means succeeded, 0 means failed
 */
int p3tmMapFB
(
   const void* i_perceptualMap,
   int         i_width,
   int         i_height,
   float*      i_inPixels,
   char*       o_outPixels,
   int*        o_asyncProgress,
   char*       o_message128
);




/*= supplementary object method (new for version 1.3) ========================*/

/**
 * Map an image (2).<br/><br/>
 *
 * @perceptualMap  object from one of the p3tmCreate___ functions
 * @width          width of input and output images
 * @height         height of input and output images
 * @inPixelsType   input pixels type, from the options/constants header
 * @inPixels       array of input RGB pixels
 * @outPixelsType  output pixels type, from the options/constants header
 * @outPixels      array of output RGB pixels
 * @asyncProgress  percentage progress feedback to be read by another thread
 * @message128     string for exception message 128 chars long, (or 0)
 *
 * @return  1 means succeeded, 0 means failed
 */
int p3tmMap2
(
   const void* perceptualMap,
   int         width,
   int         height,
   int         inPixelsType,
   void*       inPixels,
   int         outPixelsType,
   void*       outPixels,
   int*        asyncProgress,
   char*       message128
);








/*= object interface (as in version 1.1) =====================================*/

/*- basic object services ----------------------------------------------------*/
/**
 * Create a default PerceptualMap.<br/><br/>
 *
 * Defaults to: ITU-R BT.709 / sRGB input color space, cd/m^2 luminance pixel
 * values, 65 degrees view angle, ideal viewer mapping, TFT display range,
 * ITU-R BT.709 gamma.
 *
 * @return  new PerceptualMap, or 0 for failure
 */
void* p3tmCreatePerceptualMapDefault();


/**
 * Create a specified PerceptualMap.<br/><br/>
 *
 * Give zeros to set to defaults.<br/><br/>
 *
 * @inChromaticities6      array of six floats { rx, ry, gx, gy, bx, by }.
 *                         each value is >= 0 and <= 1
 *                         (default: ITU-R BT.709 / sRGB)
 * @inWhitePoint2          array of two floats, each value is > 0 and < 1
 *                         (default: ITU-R BT.709 / sRGB)
 * @inScalingAndOffset2    a transform to calibrate input pixels into cd/m^2.
 *                         array of two floats { scaling, offset }
 *                         (default: 1 and 0)
 * @inViewAngleHorizontal  horizontal angle of view frustrum in degrees
 *                         (default: 65)
 * @mappingFlags           or-able bit options for the mapping, from the
 *                         options/constants header.
 *                         (default: ideal)
 * @outLuminanceRange2     min and max display luminance (cd/m^2).
 *                         array of two floats { min, max }
 *                         (default: 2 and 150 (TFT))
 * @outGamma               power transform to apply to output pixels
 *                         (default: ITU-R BT.709: 0.45)
 *
 * @return  new PerceptualMap, or 0 for failure
 */
void* p3tmCreatePerceptualMap
(
   const float* inChromaticities6,
   const float* inWhitePoint2,
   const float* inScalingAndOffset2,
   float        inViewAngleHorizontal,
   int          mappingFlags,
   const float* outLuminanceRange2,
   float        outGamma
);


/**
 * Free a PerceptualMap.<br/><br/>
 *
 * @perceptualMap  object from one of the p3tmCreate___ functions
 *
 * @return  1 means succeeded, 0 means failed
 */
int p3tmFreePerceptualMap
(
   void* perceptualMap
);


/**
 * Create a copy of a PerceptualMap.<br/><br/>
 *
 * @other  object from one of the p3tmCreate___ functions
 *
 * @return  new PerceptualMap, or 0 for failure
 */
void* p3tmCreatePerceptualMapCopy
(
   const void* other
);


/**
 * Assign one PerceptualMap to another.<br/><br/>
 *
 * @source  object from one of the p3tmCreate___ functions
 */
void p3tmAssignPerceptualMap
(
   const void* source,
   void*       target
);




/*- commands -----------------------------------------------------------------*/
/**
 * Set the input image chromaticities and whitepoint.<br/><br/>
 *
 * Give zeros to set to defaults (ITU-R BT.709 / sRGB).<br/><br/>
 *
 * @perceptualMap      object from one of the p3tmCreate___ functions
 * @inChromaticities6  array of six floats { rx, ry, gx, gy, bx, by }
 *                     each value is >= 0 and <= 1
 * @inWhitePoint2      array of two floats, each value is > 0 and < 1
 */
void p3tmSetInputColorSpace     ( void*        perceptualMap,
                                  const float* inChromaticities6,
                                  const float* inWhitePoint2 );

/**
 * Set the input pixels transform.<br/><br/>
 *
 * @perceptualMap        object from one of the p3tmCreate___ functions
 * @inScalingAndOffset2  a transform to calibrate input pixels into cd/m^2.
 *                       array of two floats { scaling, offset }.
 *                       Give zero to set to default (1 and 0).
 */
void p3tmSetInputLuminanceScale ( void*        perceptualMap,
                                  const float* inScalingAndOffset2 );

/**
 * Set the input image view angle.<br/><br/>
 *
 * @perceptualMap          object from one of the p3tmCreate___ functions
 * @inViewAngleHorizontal  horizontal angle of view frustrum in degrees.
 *                         Give zero to set to default (65).
 */
void p3tmSetInputViewAngle      ( void* perceptualMap,
                                  float inViewAngleHorizontal );

/**
 * Set the mapping options.<br/><br/>
 *
 * @perceptualMap  object from one of the p3tmCreate___ functions
 * @mappingFlags   or-able bit options for the mapping, from the
 *                 options/constants header.
 *                 Give zero to set to default (ideal).
 */
void p3tmSetMappingFeatures     ( void* perceptualMap,
                                  int   mappingFlags );

/**
 * Set the output display range.<br/><br/>
 *
 * @perceptualMap       object from one of the p3tmCreate___ functions
 * @outLuminanceRange2  min and max display luminance (cd/m^2).
 *                      array of two floats { min, max }.
 *                      Give zero to set to default (2 and 150 (TFT)).
 */
void p3tmSetOutputLuminanceRange( void*        perceptualMap,
                                  const float* outLuminanceRange2 );


/**
 * Set the output gamma power transform.<br/><br/>
 *
 * @perceptualMap  object from one of the p3tmCreate___ functions
 * @outGamma       power transform to apply to output pixels.
 *                 Give zero to set to default (ITU-R BT.709: 0.45).
 */
void p3tmSetOutputGamma         ( void* perceptualMap,
                                  float outGamma );




/*- queries ------------------------------------------------------------------*/
/**
 * Get options.<br/><br/>
 *
 * Give zeros for values not wanted.<br/><br/>
 *
 * @perceptualMap          object from one of the p3tmCreate___ functions
 * @inChromaticities6      array of six floats { rx, ry, gx, gy, bx, by }
 * @inWhitePoint2          array of two floats
 * @inScalingAndOffset2    a transform to calibrate input pixels into cd/m^2.
 *                         array of two floats { scaling, offset }
 * @inViewAngleHorizontal  horizontal angle of view frustrum in degrees
 * @mappingFlags           or-able bit options for the mapping, from the
 *                         options/constants header.
 * @outLuminanceRange2     min and max display luminance (cd/m^2).
 *                         array of two floats { min, max }
 * @pOutGamma              power transform applied to output pixels
 */
void p3tmGetOptions
(
   const void* perceptualMap,
   float*      inChromaticities6,
   float*      inWhitePoint2,
   float*      inScalingAndOffset2,
   float*      inViewAngleHorizontal,
   int*        mappingFlags,
   float*      outLuminanceRange2,
   float*      pOutGamma
);


/**
 * Map an image.<br/><br/>
 *
 * @perceptualMap  object from one of the p3tmCreate___ functions
 * @width          width of input and output images
 * @height         height of input and output images
 * @inPixelsType   input pixels type, from the options/constants header
 * @inPixels       array of input RGB pixels
 * @outPixelsType  output pixels type, from the options/constants header
 * @outPixels      array of output RGB pixels
 * @message128     string for exception message 128 chars long, (or 0)
 *
 * @return  1 means succeeded, 0 means failed
 */
int p3tmMap
(
   const void* perceptualMap,
   int         width,
   int         height,
   int         inPixelsType,
   void*       inPixels,
   int         outPixelsType,
   void*       outPixels,
   char*       message128
);








/*= test =====================================================================*/

int p3tmTestUnits
(
   int whichTest,
   int isOutput,
   int isVerbose,
   int seed
);




#ifdef __cplusplus
} /* extern "C" */
#endif


#endif/*p3tmPerceptualMap_h*/
