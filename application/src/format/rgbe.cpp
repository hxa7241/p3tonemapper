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


#include <math.h>

#include <istream>
#include <string>
#include <vector>
#include <exception>

#include "rgbe.hpp"   /// own header is included last


using namespace p3tonemapper_format;




/*
	#?RADIANCE
	# comment
	PRIMARIES= 0.6400 0.3300 0.2900 0.6000 0.1500 0.0600 0.3333 0.3333
	FORMAT=32-bit_rle_rgbe

	-Y 512 +X 346
*/




/// constants ------------------------------------------------------------------
static const char STREAM_EXCEPTION_MESSAGE[] =
	"stream read failure, in RGBE read";




/// declarations ---------------------------------------------------------------
static bool readTaggedValues
(
	istream& in,
	float*   pPrimaries8,
	float&   exposure
);


static bool readDimensions
(
	istream& in,
	dword&   width,
	dword&   height
);


static void readImage
(
	istream&    in,
	const dword width,
	const dword height,
	const bool  isRle,
	const bool  isInverted,
	float*      pRgbTriples
);


static void convertRgbeToFloats
(
	ubyte rgbe[4],
	float rgb[3]
);




/// ----------------------------------------------------------------------------
void p3tonemapper_format::rgbe::read
(
	istream&   in,
	const bool isInvert,
	dword&     width,
	dword&     height,
	float*     pPrimaries8,
	float&     exposure,
	float*&    pRgbTriples
)
{
	pRgbTriples = 0;

	// enable stream exceptions
	std::ios_base::iostate originalExceptionFlags = in.exceptions();
	in.exceptions( istream::badbit | istream::eofbit );   // istream::failbit

	try
	{
		// read header
		bool isRle      = false;
		bool isInverted = false;
		{
			// skip ID line
			// "#?RADIANCE\0x0A"
			in.ignore( DWORD_MAX, ubyte(0x0A) );

			// read info
			isRle = readTaggedValues( in, pPrimaries8, exposure );

			// read dimensions
			isInverted = readDimensions( in, width, height );
		}

		// allocate pixels storage
		pRgbTriples = new float[ width * height * 3 ];

		// read image
		readImage( in, width, height, isRle, isInverted ^ isInvert, pRgbTriples );
	}
	catch( ... )
	{
		delete[] pRgbTriples;
		pRgbTriples = 0;

		in.exceptions( originalExceptionFlags );

		throw STREAM_EXCEPTION_MESSAGE;
	}

	in.exceptions( originalExceptionFlags );
}


bool readTaggedValues
(
	istream& in,
	float*   pPrimaries8,
	float&   exposure
)
{
	// zero outputs
	{
		for( dword i = 8;  i-- > 0; )
		{
			pPrimaries8[i] = 0.0f;
		}
		exposure = 0.0f;
	}

	// read tagged values
	bool isRle = false;
	for( ;; )
	{
		const istream::pos_type lineStart = in.tellg();

		// read tag name, up to 9 chars (followed by '=')
		char buf[] = "         ";
		in.getline( buf, sizeof(buf), '=' );
		in.clear();

		// read primaries
		if( std::string("PRIMARIES") == buf )
		{
			for( dword i = 0;  i < 8;  ++i )
			{
				in >> pPrimaries8[i];
			}
		}
		// read exposure
		else if( std::string("EXPOSURE") == buf )
		{
			// accumulate multiplication
			float e;
			in >> e;
			if( 0.0f != e )
			{
				if( 0.0f != exposure )
				{
					exposure *= e;
				}
				else
				{
					exposure = e;
				}
			}
		}
//		// read color correction
//		else if( std::string("COLORCORR") == buf )
//		{
//			// 3 floats
//			// cumulative
//		}
//		// read pixel aspect ratio
//		else if( std::string("PIXASPECT") == buf )
//		{
//			// float ?
//			// cumulative
//		}
//		// read view
//		else if( std::string("VIEW") == buf )
//		{
//			// view parameters ... ?
//			// cumulative
//		}
//		// read software
//		else if( std::string("SOFTWARE") == buf )
//		{
//			// string
//		}
		// read format into isRle
		else if( std::string("FORMAT") == buf )
		{
			char buf[] = "               ";
			in.getline( buf, sizeof(buf), char(0x0A) );
			in.clear();

			const bool isRgb = (std::string("32-bit_rle_rgbe") == buf);
			const bool isXyz = (std::string("32-bit_rle_xyze") == buf);
			isRle = isRgb | isXyz;

			if( isXyz )
			{
				// set colorspace for identity transform
				pPrimaries8[0] = 1.0f;
				pPrimaries8[3] = 1.0f;
				pPrimaries8[6] = 0.333333f;
				pPrimaries8[7] = 0.333333f;
			}

			in.seekg( lineStart );
		}
		else
		{
			// move back to start of line
			in.seekg( lineStart );

			// check if it was a blank line, which is the end of the header
			if( char(0x0A) == buf[0] )
			{
				in.ignore( 1 );
				// exit loop
				break;
			}
		}

		// ignore rest of line
		in.ignore( DWORD_MAX, ubyte(0x0A) );
	}

	return isRle;
}


bool readDimensions
(
	istream& in,
	dword&   width,
	dword&   height
)
{
	width  = 0;
	height = 0;

	// loop twice
	for( dword i = 2;  i-- > 0; )
	{
		// read - or +
		char sign;
		in >> sign;

		// read Y or X
		char axis;
		in >> axis;

		// read integer
		dword size;
		in >> size;

		size = ('-' == sign) ? -size : size;

		if( ('X' == axis) | ('x' == axis) )
		{
			width = size;
		}
		else
		{
			height = size;
		}

		// read space
		in.ignore( 1 );
	}

	 const bool isInverted = height < 0;
	 width  = width  < 0 ? -width  : width;
	 height = height < 0 ? -height : height;

	 return isInverted;


// actually different ordered X and Y means something:
//
//	+X M +Y N
//	The image is rotated 90 degrees clockwise.
//	-X M +Y N
//	The image is rotated 90 degrees clockwise, then flipped top to bottom.
//	-X M -Y N
//	The image is rotated 90 degrees ant-clockwise.
//	+X M -Y N
//	The image is rotated 90 degrees ant-clockwise, then flipped top to bottom.
}


void readImage
(
	istream&    in,
	const dword width,
	const dword height,
	const bool  isRle,
	const bool  isInverted,
	float*      pRgbTriples
)
{
	// plain
	if( !isRle | ((width < 8) | (width > 0x7FFF)) )
	{
		// read rows
		for( dword y = 0;  y < height;  ++y )
		{
			float* pPixel = pRgbTriples +
				((isInverted ? height - 1 - y : y) * width * 3);

			// read pixels
			for( dword x = 0;  x < width;  ++x, pPixel += 3 )
			{
				char rgbe[4];
				in.read( rgbe, sizeof(rgbe) );

				convertRgbeToFloats( reinterpret_cast<ubyte*>(rgbe), pPixel );
			}
		}
	}
	// run-length encoded
	else
	{
		using std::vector;
		vector<ubyte> channels[] = {
			vector<ubyte>( width ),
			vector<ubyte>( width ),
			vector<ubyte>( width ),
			vector<ubyte>( width ) };

		// read rows
		for( dword row = 0;  row < height;  ++row )
		{
			// ignore leader
			{
				in.ignore( 4 );

				// read 4 bytes
				//char bytes[4];
				//in.read( bytes, sizeof(bytes) );

				// check
				// (bytes[0] == 2) & (bytes[1] == 2) & ((bytes[2] & 0x80) == 0)
				// (dword(ubyte(bytes[2])) << 8) | dword(ubyte(bytes[3]))) == width

				// ... why bother ?
			}

			// read pixels
			{
				// read channels
				for( dword c = 0;  c < 4;  ++c )
				{
					// read runs
					ubyte* pChannel = &(channels[c][0]);
					for( dword p = 0;  p < width; )
					{
						// read run prefix byte
						const dword runPrefix = ubyte(in.get());
						const bool  isRunSame = runPrefix > 128;

						// clamp length
						dword runLength = runPrefix - (isRunSame ? 128 : 0);
						{
							const dword remaining = width - p;
							runLength = (runLength <= remaining) ? runLength : remaining;
						}

						// read or duplicate bytes
						if( isRunSame )
						{
							// read value
							const ubyte value = ubyte(in.get());

							// duplicate value
							for( dword r = runLength;  r-- > 0;  ++p )
							{
								pChannel[ p ] = value;
							}
						}
						else
						{
							// read values
							in.read( reinterpret_cast<char*>(pChannel + p), runLength );
							p += runLength;
						}
					}
				}

				// collate channels into pixels
				float* pPixel = pRgbTriples +
					((isInverted ? height - 1 - row : row) * width * 3);
				for( dword p = 0;  p < width;  ++p, pPixel += 3 )
				{
					ubyte rgbe[4];
					for( dword c = 4;  c-- > 0; )
					{
						rgbe[c] = channels[c][p];
					}

					convertRgbeToFloats( rgbe, pPixel );
				}
			}
		}
	}
}


void convertRgbeToFloats
(
	ubyte rgbe[4],
	float rgb[3]
)
{
	if( 10 < rgbe[3] )
	{
		const float a = ::ldexpf( 1.0f, dword(rgbe[3]) - (128 + 8) );

		rgb[0] = (float(rgbe[0]) + 0.5f) * a;
		rgb[1] = (float(rgbe[1]) + 0.5f) * a;
		rgb[2] = (float(rgbe[2]) + 0.5f) * a;
	}
	else
	{
		rgb[0] = 0.0f;
		rgb[1] = 0.0f;
		rgb[2] = 0.0f;
	}
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <math.h>
#include <fstream>
#include <sstream>
#include <exception>
//#include "FpToInt.hpp"
//#include "Clamps.hpp"
//#include "ppm.hpp"


static bool isRoughly
(
	const float value1,
	const float value2,
	const float tolerance = 0.00001f
);

bool isRoughly
(
	const float value1,
	const float value2,
	const float tolerance
)
{
	return ::fabsf(value1 - value2) <= (value1 * tolerance);
}


namespace p3tonemapper_format
{
namespace rgbe
{
	using namespace hxa7241;


bool test_rgbe
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_rgbe ]\n\n";


	// compare
	{
		// define fake file bytes
		static const char FAKE_HEADER[] =
			"#?RADIANCE\n"
			"PRIMARIES= 0.6400 0.3300 0.2900 0.6000 0.1500 0.0600 0.4 0.5\n"
			"EXPOSURE= 1.0\n"
			"FORMAT=32-bit_rle_rgbe\n"
			"\n"
			"-Y 3 +X 8\n";
		// R: 0 0 0 10 20 30 40 50  G: 100 100 100 100 100 110 120 130  B: 140 150 160 160 160 160 160 160  E: 170 180 190 200 210 220 230 230
		// R: 1 1 11 11 11 21 31 41  G: 51 51 51 61 71 81 81 81  B: 91 101 111 121 121 121 131 141  E: 151 161 171 171 171 171 181 181
		// R: 2 2 2 2 2 2 2 2  G: 12 22 32 42 52 62 72 82  B: 92 102 102 102 102 102 102 102  E: 112 112 112 112 112 112 112 112
		static const ubyte FAKE_PIXELS[] = {
			2, 2, 0, 8,
				131, 0, 5, 10, 20, 30, 40, 50,
				133, 100, 3, 110, 120, 130,
				2, 140, 150, 134, 160,
				6, 170, 180, 190, 200, 210, 220, 130, 230,
			2, 2, 0, 8,
				130, 1, 131, 11, 3, 21, 31, 41,
				131, 51, 2, 61, 71, 131, 81,
				3, 91, 101, 111, 131, 121, 2, 131, 141,
				2, 151, 161, 132, 171, 130, 181,
			2, 2, 0, 8,
				136, 2,
				8, 12, 22, 32, 42, 52, 62, 72, 82,
				1, 92, 135, 102,
				136, 112 };


		// put in stream
		std::stringstream strstr( std::stringstream::in | std::stringstream::out | std::stringstream::binary );
		strstr.write( reinterpret_cast<const char*>(FAKE_HEADER), sizeof(FAKE_HEADER) - 1 );
		strstr.write( reinterpret_cast<const char*>(FAKE_PIXELS), sizeof(FAKE_PIXELS) );


		// read fake file
		dword  width;
		dword  height;
		float  primaries[8];
		float  exposure;
		float* pPixelsFp = 0;
		try
		{
			p3tonemapper_format::rgbe::read(
				strstr, false, width, height,
				primaries, exposure,
				pPixelsFp );
		}
		catch( const std::exception& e )
		{
			if( pOut && isVerbose ) *pOut << "exception: " << e.what() << "\n";
		}
		catch( ... )
		{
			if( pOut && isVerbose ) *pOut << "exception \n";
		}

		// compare with target image and data
		bool isFail = false;
		static const dword WIDTH                = 8;
		static const dword HEIGHT               = 3;
		static const float PRIMARIES[8] =
			{ 0.64f, 0.33f,  0.29f, 0.6f,  0.15f, 0.06f, 0.4f, 0.5f };
		static const float EXPOSURE             = 1.0f;
		static const float PIXELS[] = {
			8.58993e+009f, 1.72658e+012f, 2.41377e+012f, 8.79609e+012f, 1.76801e+015f, 2.64762e+015f, 9.0072e+015f, 1.81045e+018f, 2.89131e+018f, 1.93691e+020f, 1.8539e+021f, 2.9607e+021f, 3.87234e+023f, 1.89839e+024f, 3.03176e+024f, 5.89956e+026f, 2.13738e+027f, 3.10452e+027f, 8.02185e+029f, 2.38675e+030f, 3.17903e+030f, 1.00026e+030f, 2.58482e+030f, 3.17903e+030f,
			49152.0f, 1.68755e+006f, 2.99827e+006f, 5.03316e+007f, 1.72805e+009f, 3.40577e+009f, 3.95137e+011f, 1.76953e+012f, 3.83111e+012f, 3.95137e+011f, 2.11312e+012f, 4.17471e+012f, 3.95137e+011f, 2.45672e+012f, 4.17471e+012f, 7.38734e+011f, 2.80032e+012f, 4.17471e+012f, 1.10831e+015f, 2.86753e+015f, 4.62674e+015f, 1.46015e+015f, 2.86753e+015f, 4.97859e+015f,
			1.49012e-007f, 7.45058e-007f, 5.51343e-006f, 1.49012e-007f, 1.3411e-006f, 6.10948e-006f, 1.49012e-007f, 1.93715e-006f, 6.10948e-006f, 1.49012e-007f, 2.5332e-006f, 6.10948e-006f, 1.49012e-007f, 3.12924e-006f, 6.10948e-006f, 1.49012e-007f, 3.72529e-006f, 6.10948e-006f, 1.49012e-007f, 4.32134e-006f, 6.10948e-006f, 1.49012e-007f, 4.91738e-006f, 6.10948e-006f
		};

		isFail |= (width != WIDTH) | (height != HEIGHT);
		if( pOut && isVerbose ) *pOut << "width: " << width << "  height: " <<
			height << "  " << isFail << "\n";

		if( pOut && isVerbose ) *pOut << "primaries: ";
		for( dword i = 0;  i < 4;  ++i )
		{
			for( dword j = 0;  j < 2;  ++j )
			{
				isFail |= !isRoughly( PRIMARIES[j + (i * 2)], primaries[j + (i * 2)] );
				if( pOut && isVerbose ) *pOut << primaries[j + (i * 2)]  << " ";
			}
		}
		if( pOut && isVerbose ) *pOut << "  " << isFail << "\n";

		isFail |= !isRoughly( exposure, EXPOSURE );
		if( pOut && isVerbose ) *pOut << "exposure: " << exposure << "  " << isFail << "\n";

		isFail |= (0 == pPixelsFp);
		if( pOut && isVerbose ) *pOut << "pixels: \n";
		if( 0 != pPixelsFp )
		{
			for( dword y = 0;  y < height;  ++y )
			{
				for( dword x = 0;  x < width;  ++x )
				{
					for( dword p = 0;  p < 3;  ++p )
					{
						const dword i = ((y * width) + x) * 3 + p;
						const dword j = (((height - y - 1) * width) + x) * 3 + p;
						isFail |= !isRoughly( pPixelsFp[i], PIXELS[j] );
						if( pOut && isVerbose ) *pOut << pPixelsFp[i] << "   " << PIXELS[j] << "   " << (pPixelsFp[i] - PIXELS[j]) << "   " << isFail << "\n";
					}
				}
			}
		}
		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "compare : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


/*	// read a file, scale the pixels, then write it as a PPM
	// (for non-automated visual inspection)

	static const char name[] = "rend04_o80A";

	std::ifstream in( (std::string("images\\") + std::string(name) + std::string(".hdr")).c_str(), std::ifstream::binary );

	dword  width;
	dword  height;
	float  primaries[8];
	float  exposure;
	float* pPixelsFp  = 0;
	byte*  pPixelsInt = 0;

	try
	{
		// read
		p3tonemapper_format::rgbe::read(
			in, false, width, height,
			primaries, exposure,
			pPixelsFp );

		pPixelsInt = new byte[ width * height * 3 ];

		// simple tonemap
		{
//			const float maxDisplayLuminance    = 100.0f;
//			const float adaptLuminanceForScene = 1.0f;
//
//			const float displayAdaptationLevel = (maxDisplayLuminance / 2.0f) / 2.0f;
//			const float sceneAdaptationLevel   = adaptLuminanceForScene;
//
//			const float a = 1.219f + float(::pow( displayAdaptationLevel, 0.4f ));
//			const float b = 1.219f + float(::pow( sceneAdaptationLevel,   0.4f ));
//
//			const float scaleFactor = float(::pow( a / b, 2.5f )) / maxDisplayLuminance;

			float* pPixelFp  = pPixelsFp;
			byte*  pPixelInt = pPixelsInt;
			for( dword p = width * height;  p-- > 0;  pPixelFp += 3, pPixelInt += 3 )
			{
				for( dword c = 3;  c-- > 0; )
				{
					pPixelInt[c] = byte( hxa7241_general::fp01ToByte(
						hxa7241_general::clamp01o( pPixelFp[c] * 10.0f ) ) );
				}
			}
		}

		// write
		std::ofstream out( (std::string(name) + std::string(".ppm")).c_str(), std::ofstream::binary );
		p3tonemapper_format::ppm::write( width, height, false, pPixelsInt, out );
	}
	catch( const std::exception& e )
	{
		if( pOut ) *pOut << "exception: " << e.what() << "\n";
	}
	catch( ... )
	{
		if( pOut ) *pOut << "exception \n";
	}

	delete[] pPixelsInt;
	delete[] pPixelsFp;


isOk = false;*/


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace
}//namespace


#endif//TESTING








/*
	const ubyte r0[] = { 0, 0, 0, 10, 20, 30, 40, 50 };
	const ubyte g0[] = { 100, 100, 100, 100, 100, 110, 120, 130 };
	const ubyte b0[] = { 140, 150, 160, 160, 160, 160, 160, 160 };
	const ubyte e0[] = { 170, 180, 190, 200, 210, 220, 230, 230 };

	const ubyte r1[] = { 1, 1, 11, 11, 11, 21, 31, 41 };
	const ubyte g1[] = { 51, 51, 51, 61, 71, 81, 81, 81 };
	const ubyte b1[] = { 91, 101, 111, 121, 121, 121, 131, 141 };
	const ubyte e1[] = { 151, 161, 171, 171, 171, 171, 181, 181 };

	const ubyte r2[] = { 2, 2, 2, 2, 2, 2, 2, 2 };
	const ubyte g2[] = { 12, 22, 32, 42, 52, 62, 72, 82 };
	const ubyte b2[] = { 92, 102, 102, 102, 102, 102, 102, 102 };
	const ubyte e2[] = { 112, 112, 112, 112, 112, 112, 112, 112 };


	for( dword i = 0;  i < 8;  ++i )
	{
		ubyte rgbe[4] = { r0[i], g0[i], b0[i], e0[i] };
		float pPixel[3];
		convertRgbeToFloats( rgbe, pPixel );
		if( pOut && isVerbose ) *pOut << pPixel[0] << ", " << pPixel[1] << ", " << pPixel[2] << ", ";
	}
	if( pOut && isVerbose ) *pOut << "\n";

	for( dword i = 0;  i < 8;  ++i )
	{
		ubyte rgbe[4] = { r1[i], g1[i], b1[i], e1[i] };
		float pPixel[3];
		convertRgbeToFloats( rgbe, pPixel );
		if( pOut && isVerbose ) *pOut << pPixel[0] << ", " << pPixel[1] << ", " << pPixel[2] << ", ";
	}
	if( pOut && isVerbose ) *pOut << "\n";

	for( dword i = 0;  i < 8;  ++i )
	{
		ubyte rgbe[4] = { r2[i], g2[i], b2[i], e2[i] };
		float pPixel[3];
		convertRgbeToFloats( rgbe, pPixel );
		if( pOut && isVerbose ) *pOut << pPixel[0] << ", " << pPixel[1] << ", " << pPixel[2] << ", ";
	}
	if( pOut && isVerbose ) *pOut << "\n";
*/
