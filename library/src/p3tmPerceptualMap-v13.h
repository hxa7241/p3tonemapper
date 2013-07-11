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
 * Options/constants for p3tmPerceptualMap interface. These will change with
 * implementation versions.<br/><br/>
 *
 * Include this file, and link with the library or access purely dynamically.
 */




#ifndef p3tmPerceptualMap_v13_h
#define p3tmPerceptualMap_v13_h


#ifdef __cplusplus
extern "C" {
#endif


#include "p3tmPerceptualMap.h"




/* version ------------------------------------------------------------------ */
/**
 * version ID
 */
enum p3tm13EVersion
{
   /* current */
   p3tm13_VERSION = 0x00010011,

   /* old and defunct */
   p3tm12_VERSION = 0x00010010,

   /* old, but included in current */
   p3tm11_VERSION = 0x00010001
};




/* mapping option flags ----------------------------------------------------- */
/**
 * Options for use with p3tmMapFBSimple(), p3tmMapFBFull(),
 * p3tmCreatePerceptualMap(), p3tmSetMappingFeatures() and p3tmGetOptions().
 * <br/><br/>
 *
 * There is a basic, optimal tonemapping: IDEAL. Various human perceptual limit
 * effects can be added on (by or-ing): CONTRAST, GLARE, COLOR, ACUITY. All
 * human effects can be selected together: HUMAN.
 *
 * CONTRAST: Absolute brightness differences are less visible in darkness --
 * so this option reduces contrast in dark regions, making a gloomier image.
 * <br/><br/>
 *
 * GLARE: (bloom not flare) Eye materials scatter a small proportion of light
 * widely over the sensors (like a wide weak filter) -- so this option reduces
 * contrast overall, and creates halos around bright areas.<br/><br/>
 *
 * COLOR: Color is less visible in darkness -- so this option desaturates
 * pixels according to luminance, making a greyish image.<br/><br/>
 *
 * ACUITY: Sharpness is reduced in darkness -- so this option filters pixels
 * according to luminance, making a blurry image.<br/><br/>
 *
 * If GLARE, COLOR, or ACUITY are used, then CONTRAST is automatically active
 * too (due to the way the algorithm works).
 */
enum p3tm11EMappingOptions
{
   /* basic, optimal */
   p3tm11_IDEAL    = 0,

   /* human perceptual limit effects. or-able with IDEAL. */
   p3tm11_CONTRAST = 1,
   p3tm11_GLARE    = 3,
   p3tm11_COLOR    = 5,
   p3tm11_ACUITY   = 9,

   /* all human effects */
   p3tm11_HUMAN    = p3tm11_CONTRAST | p3tm11_GLARE |
                     p3tm11_COLOR | p3tm11_ACUITY
};




/* pixel option flags ------------------------------------------------------- */
/**
 * Options for use with p3tmMap() inPixelsType parameter.<br/><br/>
 *
 * @p3tm11_RGB_FLOAT  pixel parts/channels are floats, in storage order R, G, B
 */
enum p3tm11EInPixelOptions
{
   p3tm11_RGB_FLOAT = 0
};


/**
 * Options for use with p3tmMap() outPixelsType parameter.<br/><br/>
 *
 * @p3tm11_RGB_BYTE  pixel parts/channels are bytes, in storage order R, G, B
 * @p3tm11_RGB_WORD  pixel parts/channels are words, in storage order R, G, B
 */
enum p3tm11EOutPixelOptions
{
   p3tm11_RGB_BYTE = 0,
   p3tm11_RGB_WORD = 1
};




#ifdef __cplusplus
} /* extern "C" */
#endif


#endif/*p3tmPerceptualMap_v13_h*/
