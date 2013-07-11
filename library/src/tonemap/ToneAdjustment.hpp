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


#ifndef ToneAdjustment_h
#define ToneAdjustment_h


#include <math.h>
#include "Interval.hpp"
#include "SamplesRegular1.hpp"

#include "hxa7241_general.hpp"
#include "p3tonemapper_image.hpp"




#include "p3tonemapper_tonemap.hpp"
namespace p3tonemapper_tonemap
{
	using hxa7241_general::Interval;
	using hxa7241_general::Histogram;
	using hxa7241_general::SamplesRegular1;
	using p3tonemapper_image::ImageRgbFloat;
	using p3tonemapper_image::ImageRgbInt;


/**
 * A cumulative function mapping of image brightness.<br/><br/>
 *
 * Constant class.
 *
 * @exceptions constructor can throw
 *
 * @see
 * Foveal
 * ImageRgbFloat
 * ImageRgbInt
 *
 * @implementation
 * 'brightness' means log10(Luminance)
 *
 * derived from the paper:
 * <cite>'A Visibility Matching Tone Reproduction Operator for High Dynamic Range
 * Scenes'
 * Ward Larson, Rushmeier, Piatko;
 * University Of California 1997.</cite>
 *
 * @invariants
 * outputLuminanceRange_m is positive
 * outputLuminanceRange_m.lower is >= hxa7241_graphics::getLuminanceMin()
 * brightnessCurve_m interval is positive
 * brightnessCurve_m interval.lower is >= hxa7241_graphics::getLuminanceMin()
 * brightnessCurve_m size > 0
 */
class ToneAdjustment
{
/// standard object services ---------------------------------------------------
public:
	         ToneAdjustment( const Foveal&,
	                         float outputLuminanceMin,
	                         float outputLuminanceMax,
	                         bool  isHumanViewer );

	virtual ~ToneAdjustment();
	         ToneAdjustment( const ToneAdjustment& );
	ToneAdjustment& operator=( const ToneAdjustment& );


/// commands -------------------------------------------------------------------


/// queries --------------------------------------------------------------------
	virtual void  map( const ImageRgbFloat&,
	                   ImageRgbInt& )                                      const;


/// implementation -------------------------------------------------------------
protected:
	// primary
	static  void  fill( const Foveal& image,
	                    Histogram&    brightnessCounts );
	static  void  adjust( bool            isHumanViewer,
	                      const Interval& outLuminanceRange,
	                      Histogram&      brightnessCounts );
	static  float mapLuminance( const SamplesRegular1& brightnessCurve,
	                            float                  inLuminance );

	// secondary
	static  float getFrequencyCeilingIdeal( float           totalSamples,
	                                        float           binWidth,
	                                        const Interval& outBrightnessRange );
	static  float getFrequencyCeilingHuman( float            totalSamples,
	                                        float            binWidth,
	                                        const Interval&  outBrightnessRange,
	                                        const Histogram& brightnessCounts,
	                                        float            inLuminance );
	static  float getContrastSensitivity( float luminance );

	// tertiary
	static  float    getBrightness( float luminance );
	static  Interval getBrightness( const Interval& luminance );

	static  float    getLuminance ( float brightness );
	static  Interval getLuminance ( const Interval& brightness );


/// fields ---------------------------------------------------------------------
private:
	Interval        outputLuminanceRange_m;
	SamplesRegular1 brightnessCurve_m;

	static const dword HISTOGRAM_SIZE;
};


}//namespace




#endif//ToneAdjustment_h
