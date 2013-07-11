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


#include "Histogram.hpp"
#include "Clamps.hpp"
#include "FpToInt.hpp"

#include "ColorConstants.hpp"

#include "ImageRgbFloat.hpp"
#include "ImageRgbInt.hpp"

#include "Foveal.hpp"

#include "ToneAdjustment.hpp"   // own header is included last


using namespace p3tonemapper_tonemap;




/// statics
const dword ToneAdjustment::HISTOGRAM_SIZE = 127;
static const char OUTPUT_LUMINANCE_RANGE_INVALID_MESSAGE[] =
	"invalid output luminance range given to ToneAdjustment constructor";




/// standard object services ---------------------------------------------------
ToneAdjustment::ToneAdjustment
(
	const Foveal& fovealImage,
	const float   outputLuminanceMin,
	const float   outputLuminanceMax,
	const bool    isHumanViewer
)
 :	outputLuminanceRange_m( outputLuminanceMin, outputLuminanceMax )
 ,	brightnessCurve_m     ()
{
	// check precondition
	{
		using hxa7241_graphics::ColorConstants::getLuminanceMin;

		if( (outputLuminanceRange_m.getRange() < 1.0f) |
			(outputLuminanceRange_m.getLower() < getLuminanceMin()) )
		{
			throw OUTPUT_LUMINANCE_RANGE_INVALID_MESSAGE;
		}
	}

	/*// validate and constrain output luminances
	{
		// if empty (including both zero), set to default
		if( outputLuminanceRange_m.isEmpty() )
		{
			using hxa7241_graphics::ColorConstants::getTftLuminanceRange;
			outputLuminanceRange_m.set(
				getTftLuminanceRange()[0], getTftLuminanceRange()[1] );
		}

		// make positive
		if( outputLuminanceRange_m.isNegative() )
		{
			outputLuminanceRange_m = outputLuminanceRange_m.getNegative();
		}
		else if( outputLuminanceRange_m.isEmpty() )
		{
			// (wont trust the default)
			outputLuminanceRange_m.set( 2.0f, 80.0f );
		}

		// clamp to a minimum (shift interval up)
		using hxa7241_graphics::ColorConstants::getLuminanceMin;
		if( getLuminanceMin() > outputLuminanceRange_m.getLower() )
		{
			outputLuminanceRange_m.set(
				getLuminanceMin(),
				outputLuminanceRange_m.getUpper() +
					(getLuminanceMin() - outputLuminanceRange_m.getLower()) );
		}
	}*/

	// make preliminary histogram
	Histogram brightnessCounts;
	{
		ToneAdjustment::fill( fovealImage, brightnessCounts );

		ToneAdjustment::adjust( isHumanViewer, outputLuminanceRange_m,
			brightnessCounts );
	}

	// make mapping curve
	brightnessCurve_m.setCumulative( brightnessCounts.getBins(),
		brightnessCounts.getXAxis(), getBrightness(outputLuminanceRange_m) );
}


ToneAdjustment::~ToneAdjustment()
{
}


ToneAdjustment::ToneAdjustment
(
	const ToneAdjustment& other
)
{
	ToneAdjustment::operator=( other );
}


ToneAdjustment& ToneAdjustment::operator=
(
	const ToneAdjustment& other
)
{
	if( &other != this )
	{
		outputLuminanceRange_m = other.outputLuminanceRange_m;
		brightnessCurve_m      = other.brightnessCurve_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------




/// queries --------------------------------------------------------------------
void ToneAdjustment::map
(
	const ImageRgbFloat& inImage,
	ImageRgbInt&         outImage
) const
{
	// check images same size (length will do for this...)
	if( inImage.getLength() == outImage.getLength() )
	{
		using hxa7241_graphics::Vector3f;
		using p3tonemapper_image::ColorSpace;

		const ColorSpace& colorSpace = inImage.getColorSpace();

		// loop through pixels
		for( dword i = 0;  i < outImage.getLength();  ++i )
		{
			const Vector3f& inPixel = inImage.get( i );

			// get luminance of pixel
			float inLuminance = colorSpace.getRgbLuminance( inPixel );
			hxa7241_general::clampMin( inLuminance,
				hxa7241_graphics::ColorConstants::getLuminanceMin() );

			// map luminance with curve
			const float outLuminance =
				mapLuminance( brightnessCurve_m, inLuminance );

			// rescale pixel to 0-1
			float rgb01[3];
			{
				const float out01 =
					outputLuminanceRange_m.getInterpolantClamped( outLuminance );

				// calc scaling to bring input pixel into 0-1 range
				const float scaling = out01 / inLuminance;

				// scale pixel
				Vector3f outPixel( inPixel * scaling );
				outPixel.getXYZ( rgb01 );
			}

			// write to output pixel
			// (clamp and quantize into something like 16 bits)
			// (clamp desaturates color at ends of range)
			outImage.setElement( i, rgb01 );
		}
	}
}




/// implementation -------------------------------------------------------------
void ToneAdjustment::fill
(
	const Foveal& fovealImage,
	Histogram&    brightnessCounts
)
{
	// check input not degenerate
	if( 0 < fovealImage.getLength() )
	{
		using hxa7241_graphics::Vector3f;
		using p3tonemapper_image::ColorSpace;

		const ColorSpace& colorSpace = fovealImage.getColorSpace();

		// find min and max luminances (above special minimum)
		using hxa7241_graphics::ColorConstants::getLuminanceMin;
		Interval luminanceRange( FLOAT_MAX, getLuminanceMin() );
		{
			// loop through foveal image
			for( dword i = fovealImage.getLength();  i-- > 0; )
			{
				float luminance = colorSpace.getRgbLuminance(
					fovealImage.get( i ) );
				hxa7241_general::clampMin( luminance, getLuminanceMin() );

				luminanceRange.ratchetMinMax( luminance );
			}
		}

		// set histogram axis
		brightnessCounts.setXAxis( getBrightness( luminanceRange ) );

		// fill histogram
		// check all pixels are not same
		if( !brightnessCounts.getXAxis().isEmpty() )
		{
			brightnessCounts.setSize( HISTOGRAM_SIZE );

			// loop through foveal image
			for( dword i = fovealImage.getLength();  i-- > 0; )
			{
				// get brightness of pixel
				const float luminance  = colorSpace.getRgbLuminance(
					fovealImage.get( i ) );
				const float brightness = getBrightness( luminance );

				// add to histogram
				brightnessCounts.addToBin( brightness, 1.0f );
			}
		}
		// all pixels same
		else
		{
			// make one-bin histogram containing image length
			brightnessCounts.setSize( 1 );
			brightnessCounts.setAllBins( float(fovealImage.getLength()) );
		}
	}
	// empty histogram
	else
	{
		brightnessCounts.setSize( 0 );
		brightnessCounts.setXAxis( Interval::ZERO() );
	}
}


void ToneAdjustment::adjust
(
	const bool      isHumanViewer,
	const Interval& outLuminanceRange,
	Histogram&      brightnessCounts
)
{
	// can only operate on histograms longer than 1
	if( 1 < brightnessCounts.getSize() )
	{
		const Interval outBrightnessRange( getBrightness( outLuminanceRange ) );

		// check if brightness range already fits in display
		const bool isFitsInDisplay =
			brightnessCounts.getXAxis() <= outBrightnessRange;

		bool isConverged = false;
		if( !isFitsInDisplay )
		{
			const float binWidth = brightnessCounts.getBinWidth();

			// iterate to convergence
			const dword tolerance = hxa7241_general::round(
				0.025f * brightnessCounts.getTotal() );
			for( ; ; )
			{
				const float totalSamples = brightnessCounts.getTotal();
				float       trimmings    = 0;

				if( totalSamples < tolerance )
				{
					// exit iteration, failing
					isConverged = false;
					break;
				}

				const Histogram brightnessCountsLast( brightnessCounts );

				// loop through bins
				float inBrightness = brightnessCounts.getXAxis().getLower() +
					(binWidth * 0.5f);
				float* pBin = brightnessCounts.getBins().getMemory();
				float* pEnd = pBin + brightnessCounts.getSize();
				for( ;  pBin < pEnd;  ++pBin )
				{
					// calc ceiling
					float ceiling = FLOAT_MAX;
					if( isHumanViewer )
					{
						ceiling = getFrequencyCeilingHuman( totalSamples, binWidth,
							outBrightnessRange, brightnessCountsLast,
							getLuminance(inBrightness) );
					}
					else
					{
						ceiling = getFrequencyCeilingIdeal( totalSamples, binWidth,
							outBrightnessRange );
					}

					// if bin count larger than ceiling
					const float count = *pBin;// >= 0.0f ? *pBin : 0.0f;
					if( count > ceiling )
					{
						// clamp count and save trimming
						trimmings += count - ceiling;
						*pBin = ceiling;
					}

					inBrightness += binWidth;
				}

				if( trimmings <= tolerance )
				{
					// exit iteration, succeeding
					isConverged = true;
					break;
				}
			}
		}

		// if adjustment wasnt done
		if( isFitsInDisplay | !isConverged )
		{
			// setup simple linear mapping
			// filling all counts with the same >=1 constant value will induce this
			// when histogram is converted to cumulative function (integrated)
			brightnessCounts.setAllBins( 1.0f );
		}
	}
}


float ToneAdjustment::mapLuminance
(
	const SamplesRegular1& brightnessCurve,
	const float            inLuminance
)
{
	// convert in luminance to brightness
	const float inBrightness = getBrightness( inLuminance );

	// map with curve
	const float outBrightness = brightnessCurve.getValue( inBrightness );

	// convert out brightness to luminance
	float outLuminance = getLuminance( outBrightness );
	hxa7241_general::clampMin( outLuminance,
		hxa7241_graphics::ColorConstants::getLuminanceMin() );

	return outLuminance;
}




float ToneAdjustment::getFrequencyCeilingIdeal
(
	const float     totalSamples,
	const float     binWidth,
	const Interval& outBrightnessRange
)
{
	const float rangesRatio = (totalSamples * binWidth) /
		outBrightnessRange.getRange();

	return float( hxa7241_general::round( rangesRatio ) );
}


float ToneAdjustment::getFrequencyCeilingHuman
(
	const float      totalSamples,
	const float      binWidth,
	const Interval&  outBrightnessRange,
	const Histogram& brightnessCounts,
	const float      inLuminance
)
{
	SamplesRegular1 brightnessCurve;
	brightnessCurve.setCumulative( brightnessCounts.getBins(),
		brightnessCounts.getXAxis(), outBrightnessRange );

	const float outLuminance = ToneAdjustment::mapLuminance(
		brightnessCurve, inLuminance );

	const float contrastSensitivityRatio =
		getContrastSensitivity( outLuminance ) /
		getContrastSensitivity( inLuminance );

	const float rangesRatio = (totalSamples * binWidth) /
		outBrightnessRange.getRange();

	const float luminanceRatio = inLuminance / outLuminance;

	return float( hxa7241_general::round(
		contrastSensitivityRatio * rangesRatio * luminanceRatio ) );
}


float ToneAdjustment::getContrastSensitivity
(
	const float luminance
)
{
	float brightnessContrastSensitivity = 0.0f;

	const float brightness = getBrightness( luminance );
	if( -3.94f > brightness )
	{
		brightnessContrastSensitivity = -2.86f;
	}
	else if( -1.44f > brightness )
	{
		brightnessContrastSensitivity =
			::powf(0.405f * brightness + 1.6f, 2.18f) - 2.86f;
	}
	else if( -0.0184f > brightness )
	{
		brightnessContrastSensitivity = brightness - 0.395f;
	}
	else if( 1.9f > brightness )
	{
		brightnessContrastSensitivity =
			::powf(0.249f * brightness + 0.65f, 2.7f) - 0.72f;
	}
	else
	{
		brightnessContrastSensitivity = brightness - 1.255f;
	}

	return getLuminance( brightnessContrastSensitivity );
}




float ToneAdjustment::getBrightness
(
	const float luminance
)
{
	const float min = hxa7241_graphics::ColorConstants::getLuminanceMin();
	return ::log10f( luminance >= min ? luminance : min );
}


Interval ToneAdjustment::getBrightness
(
	const Interval& luminance
)
{
	return Interval(
		getBrightness( luminance.getLower() ),
		getBrightness( luminance.getUpper() ) );
}


float ToneAdjustment::getLuminance
(
	const float brightness
)
{
	return ::powf( 10.0f, brightness );
}


Interval ToneAdjustment::getLuminance
(
	const Interval& brightness
)
{
	return Interval(
		getLuminance( brightness.getLower() ),
		getLuminance( brightness.getUpper() ) );
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>
#include <fstream>
//#include <math.h>

//#include "rgbe.hpp"
//#include "ppm.hpp"


namespace p3tonemapper_tonemap
{
	using namespace hxa7241;


class ToneAdjustmentTest
	: public ToneAdjustment
{
private:
	         ToneAdjustmentTest();

public:
	static  bool test( std::ostream* pOut,
	                   bool    isVerbose,
                      dword   seed );
};


bool ToneAdjustmentTest::test
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_ToneAdjustment ]\n\n";


	// fill
	{
		// make image
		ImageRgbFloat image( 40, 25 );
		const p3tonemapper_image::ColorSpace& colorSpace =
			image.getColorSpace();
		for( dword i = image.getLength();  i-- > 0; )
		{
			// ramp over human-ish range
			const float y = (float(i) / float(image.getLength())) * (1e+6f - 1e-4f)
				+ 1e-4f;
			image.set( i, hxa7241_graphics::Vector3f( y, y, y ) );
		}

		// calc stats on image: count, mean, max, min, sum, ...
		const float count = float(image.getLength());
		float sum = 0.0f;
		float min = FLOAT_MAX;
		float max = FLOAT_MIN_NEG;
		for( dword i = image.getLength();  i-- > 0; )
		{
			const float brightness = getBrightness(
					colorSpace.getRgbLuminance( image.get(i) ) );

			sum += brightness;
			min = min > brightness ? brightness : min;
			max = max < brightness ? brightness : max;
		}
		//const float mean = sum / count;

		// call fill
		Foveal foveal( image, 70.0f );
		Histogram histogram;
		ToneAdjustment::fill( foveal, histogram );

		// compare with results
		bool isFail = false;
		{
			isFail |= (foveal.getLength() != image.getLength());
			isFail |= histogram.getSize() < 100;

			isFail |= histogram.getXAxis().getLower() != min;
			isFail |= histogram.getXAxis().getUpper() != max;
			isFail |= histogram.getTotal() != count;

			if( pOut && isVerbose ) *pOut <<
				min << " " << histogram.getXAxis().getLower() << "  " <<
				max << " " << histogram.getXAxis().getUpper() << "  " <<
				count << " " << histogram.getTotal() << "  " << !isFail << "\n";

			float sumLower = 0.0f;
			float sumUpper = 0.0f;
			const hxa7241_general::Array<float>& bins = histogram.getBins();
			for( dword i = bins.getLength();  i-- > 0; )
			{
				sumLower += bins[i] * (histogram.getBinWidth() * float(i) +
					histogram.getXAxis().getLower());
				sumUpper += bins[i] * (histogram.getBinWidth() * float(i + 1) +
					histogram.getXAxis().getLower());
			}

			isFail |= (sum < sumLower) | (sum > sumUpper);

			if( pOut && isVerbose ) *pOut << sum << "  " <<
				sumLower << "  " << sumUpper << "  " << !isFail << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "fill : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// brightness <-> luminance
	{
		static const float luminances[] = {
			0.0001f, 0.001f, 1.0f, 100.0f, 10000.0f, 1e+7f,
			0.0003792f, 0.0093185f, 4.7234f, 652.8f, 77210.0f, 8.2016e+7f };

		bool isFail = false;
		for( dword i = 0;  i < dword(sizeof(luminances)/sizeof(luminances[0]));  ++i )
		{
			const Interval iL( luminances[i], luminances[i] );
			const Interval iB( getBrightness( iL ) );
			const Interval iBL( getLuminance( iB ) );

			isFail |= ::fabsf(iL.getLower() - iBL.getLower()) > (iL.getLower() * 1e-5f);
			isFail |= ::fabsf(iL.getUpper() - iBL.getUpper()) > (iL.getLower() * 1e-5f);

			if( pOut && isVerbose ) *pOut << iL.getLower() << "  " <<
				iB.getLower() << "  " << iBL.getLower() << "  " <<
				((iL.getLower() - iBL.getLower()) / iL.getLower()) << "  " << !isFail << "\n";
		}

		const float min = hxa7241_graphics::ColorConstants::getLuminanceMin();
		isFail |= ::fabsf(getLuminance(getBrightness( 0.0f ))   - min) > 1e-5f;
		isFail |= ::fabsf(getLuminance(getBrightness( 1e-7f ))  - min) > 1e-5f;
		isFail |= ::fabsf(getLuminance(getBrightness( -47.0f )) - min) > 1e-5f;

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "brightness : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// getContrastSensitivity
	{
		bool isFail = false;

		float last = 0.0f;
		for( float brightness = -4.0f;  brightness < 2.0f;
			brightness += 6.0f / 1000.0f )
		{
			const float luminance = ::powf( 10.0f, brightness );
			const float cs = getContrastSensitivity( luminance );

			isFail |= ((cs - last) < -1e-3f) | (cs < 0.0f);

			//if( pOut && isVerbose ) *pOut << luminance << "  " << cs << "  " <<
			//	!isFail << "\n";

			last = cs;
		}

		isFail |= getContrastSensitivity( 0.0f ) <= 0.0f;
		isFail |= getContrastSensitivity( -100.0f ) <= 0.0f;

		//if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "constrast : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// adjust
	{
		using hxa7241_graphics::ColorConstants::getTftLuminanceRange;
		const Interval outLuminanceRange(
			getTftLuminanceRange()[0], getTftLuminanceRange()[1] );
		Histogram brightnessCounts;

		brightnessCounts.setSize( 127 );
		brightnessCounts.setXAxis( Interval( 0.5f, 3.5f ) );
		brightnessCounts.setAllBins( 40.0f );

		hxa7241_general::Array<float>& bins = brightnessCounts.getBins();

		// put three sharp peaks in
		const dword starts[]  = { 20, 40, 80 };
		const float heights[] = { 500.0f, 200.0f, 300.0f };
		//const float heights[] = { 50.0f, 60.0f, 70.0f };
		const dword width     = 9;
		const dword peak      = width / 2;
		for( dword i = 0;  i < dword(sizeof(starts)/sizeof(starts[0]));  ++i )
		{
			const dword start  = starts[i];
			const float height = heights[i];

			for( dword j = 0;  j < width;  ++j )
			{
				bins[ j + start ] =
					(float(peak - ::abs( j - peak )) / float(peak)) * height;
			}
		}
		/*for( dword i = brightnessCounts.getSize();  i-- > 0; )
		{
			bins[i] = float(i);
		}*/

		// check changed values of output are equal to maximum
		// or all values equal 1
		bool isFail = false;
		{
			const bool isHumanViewer  = false;
			const float originalTotal = brightnessCounts.getTotal();
			const Interval outBrightnessRange(
				ToneAdjustment::getBrightness( outLuminanceRange ) );

			const Histogram oldCounts( brightnessCounts );
			const hxa7241_general::Array<float>& oldBins = oldCounts.getBins();
			ToneAdjustment::adjust(
				isHumanViewer, outLuminanceRange, brightnessCounts );

			// estimate the last ceiling used in adjust
			const dword tolerance = hxa7241_general::round(
				0.025f * originalTotal );
			const float ceilingLow  = ToneAdjustment::getFrequencyCeilingIdeal(
				brightnessCounts.getTotal(),
				brightnessCounts.getBinWidth(),
				outBrightnessRange );
			const float ceilingHigh = ToneAdjustment::getFrequencyCeilingIdeal(
				brightnessCounts.getTotal() + tolerance,
				brightnessCounts.getBinWidth(),
				outBrightnessRange );

			// get max
			float max = FLOAT_MIN_NEG;
			for( dword i = 0;  i < bins.getLength();  ++i )
			{
				max = max < bins[i] ? bins[i] : max;
			}

			bool isAllOne = true;
			for( dword i = 0;  i < bins.getLength();  ++i )
			{
				const bool isChanged = oldBins[i] != bins[i];
				if( isChanged )
				{
					isFail |= bins[i] != max;
					isFail |= (bins[i] < ceilingLow) | (bins[i] > ceilingHigh);
				}

				isAllOne &= (1.0f == bins[i]);

				if( pOut && isVerbose ) *pOut << bins[i] << "   " <<
					oldBins[i] << "  " << isChanged << "  " << isFail << "\n";
			}
			if( pOut && isVerbose ) *pOut << "  " << max << "  " << ceilingLow << " " << ceilingHigh << "  " << isAllOne << "\n";

			// reclaim success if all values equal one
			isFail = isFail & !isAllOne;
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "adjust : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	/*// human ceiling
	{
		const Interval outLuminanceRange(
			hxa7241_graphics::getTftLuminanceRange()[0],
			hxa7241_graphics::getTftLuminanceRange()[1] );
		Histogram brightnessCounts;

		brightnessCounts.setSize( 127 );
		//brightnessCounts.setXAxis( Interval( 0.5f, 3.5f ) );
		brightnessCounts.setXAxis( Interval( -2.0f, 3.0f ) );
		//brightnessCounts.setXAxis( Interval( -4.0f, 1.0f ) );
		brightnessCounts.setAllBins( 40.0f );

		const hxa7241_general::Array<float>& bins = brightnessCounts.getBins();

		// put three sharp peaks in
		const dword starts[]  = { 20, 40, 80 };
		const float heights[] = { 500.0f, 200.0f, 300.0f };
		//const float heights[] = { 50.0f, 60.0f, 70.0f };
		const dword width     = 9;
		const dword peak      = width / 2;
		for( dword i = 0;  i < sizeof(starts)/sizeof(starts[0]);  ++i )
		{
			const dword start  = starts[i];
			const float height = heights[i];

			for( dword j = 0;  j < width;  ++j )
			{
				bins[ j + start ] =
					(float(peak - ::abs( j - peak )) / float(peak)) * height;
			}
		}
//		for( dword i = brightnessCounts.getSize();  i-- > 0; )
//		{
//			bins[i] = float(i);
//		}

//		bool isHumanViewer = true;
//		ToneAdjustment::adjust(
//			isHumanViewer, outLuminanceRange, brightnessCounts );
//
//		for( dword i = 0;  i < bins.getLength();  ++i )
//		{
//			if( pOut && isVerbose ) *pOut << bins[i] << "\n";
//		}


		const float totalSamples = brightnessCounts.getTotal();
		const float binWidth     = brightnessCounts.getBinWidth();
		const Interval outBrightnessRange(
			ToneAdjustment::getBrightness( outLuminanceRange ) );
		float inBrightness = brightnessCounts.getXAxis().getLower() +
			(binWidth * 0.5f);
		for( dword i = 0;  i < bins.getLength();  ++i )
		{
			const float ceiling = getFrequencyCeilingHuman(
				totalSamples, binWidth,
				outBrightnessRange, brightnessCounts,
				getLuminance(inBrightness) );

			if( pOut && isVerbose ) *pOut << i << "  " << bins[i] << "  " <<
				ceiling << "  " << getLuminance(inBrightness) << "\n";

			inBrightness += binWidth;
		}
	}*/


	// map
	/*{
		// identity: in and out luminance ranges same
		// ...

		// ramp
		bool isFail = false;
		{
			// human or ideal
			for( dword h = 0;  h < 2;  ++h )
			{
				// different input ranges
				for( dword i = -5, s = 3;  i < 7;  i += s )
				{
					// different input distributions
					for( dword j = 0;  j < 4;  ++j )
					{
						static const dword IW = 100;
						const ImageRgbFloat imageIn( IW, IW + 1 );
						const p3tonemapper_image::ColorSpace& colTrans =
							imageIn.getColorSpace();
						{
							const float lo = ::powf( 10.0f, float(i) );
							const float hi = ::powf( 10.0f, float(i + s) );

							// first row is ramp
							for( dword x = 0;  x < imageIn.getWidth();  ++x )
							{
								const float a = float(x) / float(imageIn.getWidth() -1);
								const float lum = (lo * (1.0f - a)) + (a * hi);

								imageIn.set( x, 0, colTrans.transXyzToRgb_(
									hxa7241_graphics::Vector3f(lum, lum, lum) ) );
							}

							// other pixels weight the frequency of each ramp value
							{
								static const dword DL = 11;
								static const dword flats[DL]   =
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
								static const dword spikys[DL]  =
									{ 1, 2, 1, 100, 1, 2, 1, 200, 1, 150, 1 };
								static const dword linears[DL] =
									{ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
								static const dword normals[DL] =
									{ 1, 11, 30, 70, 90, 100, 90, 70, 30, 11, 1 };
								const dword* ppDistributions[] =
									{ flats, spikys, linears, normals };

								if( pOut ) *pOut << "    ";
								float interpolations[IW];
								float sum = 0.0f;
								for( dword p = 0;  p < IW;  ++p )
								{
									const float df  = float(DL - 1) * (float(p) / float(IW));
									const dword di  = dword(df);
									const float dii = df - float(di);

									interpolations[p] = float((ppDistributions[j])[di]) +
										(dii * float((ppDistributions[j])[di + 1] -
										(ppDistributions[j])[di]));
									sum += interpolations[p];

									//if( pOut ) *pOut << interpolations[p] << "  ";
									//if( pOut ) *pOut << "(" << di << " " << dii << ")  ";
								}
								if( pOut ) *pOut << "\n";

								dword counts[IW];
								for( dword p = 0;  p < IW;  ++p )
								{
									counts[p] = hxa7241_general::round(
										float(IW * IW) * (interpolations[p] / sum) );
									counts[p] += p > 0 ? counts[p - 1] : 0;
								}

								for( dword p = 0, k = 0;  p < (IW * IW);  ++p )
								{
									if( p >= counts[k] )
									{
										++k;
									}
									imageIn.set( p + IW, imageIn.get(k) );
								}
							}
						}


//						if( (0 == h) & (1 == i) )
//						{
//							for( dword y = 0;  y < 11;  ++y )
//							{
//								for( dword x = 0;  x < 10;  ++x )
//								{
//									if( pOut ) *pOut << colTrans.getRgbLuminance( imageIn.get( x, y ) ) << " ";
//								}
//								if( pOut ) *pOut << "\n";
//							}
//							if( pOut ) *pOut << "\n";
//
//							Histogram hist( 10, Interval(10.0f, 10000.0f) );
//							for( dword p = 0;  p < 110;  ++p )
//							{
//								const float lum = colTrans.getRgbLuminance( imageIn.get(p) );
//								hist.addToBin( lum, 1.0f );
//							}
//							const hxa7241_general::Array<float>& bins = hist.getBins();
//							for( dword p = 0;  p < bins.getLength();  ++p )
//							{
//								if( pOut ) *pOut << bins[p] << " ";
//							}
//							if( pOut ) *pOut << "\n\n";
//						}


						const Foveal foveal( imageIn, 63.5f );
						const float outputLuminanceMin = 1.0f;
						const float outputLuminanceMax = 100.0f;
						const bool  isHumanViewer      = (1 == h);


//							for( dword y = 0;  y < foveal.getHeight();  ++y )
//							{
//								for( dword x = 0;  x < foveal.getWidth();  ++x )
//								{
//									if( pOut ) *pOut << "  " << colTrans.getRgbLuminance( foveal.get( x, y ) ) << " ";
//								}
//								if( pOut ) *pOut << "\n";
//							}
//							if( pOut ) *pOut << "\n";


						const ToneAdjustment adjustment(
							foveal, outputLuminanceMin, outputLuminanceMax, isHumanViewer );

						ImageRgbInt imageOut( IW, IW + 1 );
						adjustment.map( imageIn, imageOut );

						// ramp is monotonic
						// min -> 0, max -> 1
						{
							if( pOut ) *pOut << h << " " << i << " " << j << "\n";
							for( dword p = 0;  p < IW;  ++p )
							{
								//const float lum = colTrans.getRgbLuminance( imageIn.get(p,0) );
								//if( pOut ) *pOut << lum << " ";
								if( pOut ) *pOut << imageIn.get(p,0).getY() << " ";
							}
							if( pOut ) *pOut << "\n";
							for( dword p = 0;  p < IW;  ++p )
							{
								//const float lum = colTrans.getRgbLuminance( imageOut.get(p,0) );
								//if( pOut ) *pOut << lum << " ";
								if( pOut ) *pOut << uword(imageOut.get(p,0).getY()) << " ";
							}
							if( pOut ) *pOut << "\n\n";
						}


//						{
//							Histogram histogram;
//							ToneAdjustment::fill( foveal, histogram );
//							const hxa7241_general::Array<float>& bins = histogram.getBins();
//							for( dword p = 0;  p < bins.getLength();  ++p )
//							{
//								if( pOut ) *pOut << bins[p] << " ";
//							}
//							if( pOut ) *pOut << "\n\n";
//						}
					}
					if( pOut ) *pOut << "\n";
				}
			}
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "map : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}*/


	/*{
		using hxa7241_graphics::Vector3f;
		using p3tonemapper_image::ColorSpace;

		// read image
		std::ifstream in( "images\\rend11_o972.hdr", std::ifstream::binary );
		dword  width;
		dword  height;
		float  chromaticities[6];
		float  whitePoint[2];
		float  gamma;
		float  exposure;
		float* pRgbTriples;
		p3tonemapper_format::rgbe::read( in, false, width, height,
			chromaticities, whitePoint, gamma, exposure, pRgbTriples );

		const ColorSpace    conversion( chromaticities, whitePoint );
		const ImageRgbFloat inImage( width, height, pRgbTriples, conversion );

for( dword i = inImage.getLength();  i-- > 0; )
{
	inImage.set( i, inImage.get(i) *= 100.0f );
}

		// make foveal
		Foveal foveal( inImage, 43.0f );

		Interval outputLuminanceRange( 2.0f, 150.0f );

		// make histogram
		Histogram brightnessCounts;
		ToneAdjustment::fill( foveal, brightnessCounts );
		ToneAdjustment::adjust( false, outputLuminanceRange, brightnessCounts );

		// make mapping curve
		SamplesRegular1 brightnessCurve;
		brightnessCurve.setCumulative( brightnessCounts.getBins(),
			brightnessCounts.getXAxis(), Interval(0.0f, 99.0f) );

		// make histogram
		Histogram brightnessCounts1;
		ToneAdjustment::fill( foveal, brightnessCounts1 );
		ToneAdjustment::adjust( true, outputLuminanceRange, brightnessCounts1 );

		// make mapping curve
		SamplesRegular1 brightnessCurve1;
		brightnessCurve1.setCumulative( brightnessCounts1.getBins(),
			brightnessCounts1.getXAxis(), Interval(0.0f, 99.0f) );

		// make log image
		hxa7241_general::Array<ubyte> logImage( 128 * 100 * 3 );
		logImage.zeroMemory();

		// draw graph of histogram
		const hxa7241_general::Array<float>& bins  = brightnessCounts.getBins();
		const hxa7241_general::Array<float>& bins1 = brightnessCounts1.getBins();
		float binMax = 0.0f;
		for( dword i = 0;  i < bins.getLength();  ++i )
		{
			binMax = binMax >= bins[i] ? binMax : bins[i];
		}
		for( dword i = 0;  i < bins.getLength();  ++i )
		{
			for( dword y = dword(bins[i] * (99.0f / binMax)) + 1;  y-- > 0; )
			{
				const dword offset = (i + (y * 128)) * 3;
				logImage[ offset + 0 ] += 128;
			}

			for( dword y = dword(bins1[i] * (99.0f / binMax)) + 1;  y-- > 0; )
			{
				const dword offset = (i + (y * 128)) * 3;
				logImage[ offset + 1 ] += 128;
			}
		}

		// draw graph of curve
		const hxa7241_general::Array<float>& samps  = brightnessCurve.getSamples();
		const hxa7241_general::Array<float>& samps1 = brightnessCurve1.getSamples();
		for( dword i = 0;  i < samps.getLength();  ++i )
		{
			const dword offset = (i + (dword(samps[i]) * 128)) * 3;
			logImage[ offset + 0 ] = 255;

			const dword offset1 = (i + (dword(samps1[i]) * 128)) * 3;
			logImage[ offset1 + 1 ] = 255;
		}

		// write image
		std::ofstream out( "zzz.ppm", std::ofstream::binary );
		p3tonemapper_format::ppm::write( 128, 100, false,
			static_cast<const byte*>(logImage.getMemory()), out );
	}*/


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


bool test_ToneAdjustment
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   seed
)
{
	return ToneAdjustmentTest::test( pOut, isVerbose, seed );
}


}//namespace


#endif//TESTING
