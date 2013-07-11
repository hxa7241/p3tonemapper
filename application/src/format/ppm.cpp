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


#include <ostream>
#include "StringConstants.hpp"

#include "ppm.hpp"   /// own header is included last


using namespace p3tonemapper_format;




/*

http://netpbm.sourceforge.net/doc/ppm.html

Each PPM image consists of the following:

* A "magic number" for identifying the file type. A ppm image's magic number is
  the two characters "P6".
* Whitespace (blanks, TABs, CRs, LFs).
* A width, formatted as ASCII characters in decimal.
* Whitespace.
* A height, again in ASCII decimal.
* Whitespace.
* The maximum color value (Maxval), again in ASCII decimal. Must be less than
  65536 and more than zero.
* Newline or other single whitespace character.
* A raster of Height rows, in order from top to bottom. Each row consists of
  Width pixels, in order from left to right. Each pixel is a triplet of red,
  green, and blue samples, in that order. Each sample is represented in pure
  binary by either 1 or 2 bytes. If the Maxval is less than 256, it is 1 byte.
  Otherwise, it is 2 bytes. The most significant byte is first.
  A row of an image is horizontal. A column is vertical. The pixels in the image
  are square and contiguous.
* In the raster, the sample values are "nonlinear." They are proportional to the
  intensity of the ITU-R Recommendation BT.709 red, green, and blue in the
  pixel, adjusted by the BT.709 gamma transfer function. (That transfer function
  specifies a gamma number of 2.2 and has a linear section for small
  intensities). A value of Maxval for all three samples represents CIE D65 white
  and the most intense color in the color universe of which the image is part
  (the color universe is all the colors in all images to which this image might
  be compared).
  ITU-R Recommendation BT.709 is a renaming of the former CCIR Recommendation
  709. When CCIR was absorbed into its parent organization, the ITU, ca. 2000,
  the standard was renamed. This document once referred to the standard as CIE
  Rec. 709, but it isn't clear now that CIE ever sponsored such a standard.
  Note that another popular color space is the newer sRGB. A common variation on
  PPM is to subsitute this color space for the one specified.
* Note that a common variation on the PPM format is to have the sample values be
  "linear," i.e. as specified above except without the gamma adjustment.
  pnmgamma takes such a PPM variant as input and produces a true PPM as output.
* Characters from a "#" to the next end-of-line, before the maxval line, are
  comments and are ignored.

*/




/// constants ------------------------------------------------------------------
static const char PPM_ID[]  = "P6";

static const char STREAM_EXCEPTION_MESSAGE[] =
	"stream write failure, in PPM write";




//void p3tonemapper_format::ppm::read
//(
//	istream&    in,
//	const dword orderingFlags,
//	const bool  is48Bit,
//	dword&      width,
//	dword&      height,
//	void*&      pTriples
//)
//{
//	// check ID
//
//	// (skip comments)
//
//	// read width
//	// read height
//
//	// read maxval
//
//	// allocate bytes
//
//	// read rows (top first)
//		// read pixels (left first)
//			// read channels (R then G then B)
//				// read value (most significant byte first)
//
//	// ignore implicit colorspace, gamma, etc.
//}


void p3tonemapper_format::ppm::write
(
	dword       width,
	dword       height,
	const bool  is48Bit,
	const dword orderingFlags,
	const void* pTriples,
	ostream&    out
)
{
	// enable stream exceptions
	std::ios_base::iostate originalExceptionFlags = out.exceptions();
	out.exceptions( ostream::badbit | ostream::failbit | ostream::eofbit );

	try
	{
		// clamp dimensions to positive
		if( width < 0 )
		{
			width = 0;
		}
		if( height < 0 )
		{
			height = 0;
		}

		// write header
		{
			// write ID
			out << PPM_ID << '\n';
			out << "# " << hxa7241_general::HXA7241_URI() << "\n\n";

			// write width, height, maxval
			out << width <<  ' ' << height << '\n';
			out << (is48Bit ? 65535 : 255) << '\n';
		}

		// write pixels
		if( 0 != pTriples)
		{
			// write rows (top first)
			for( dword y = height;  y-- > 0; )
			{
				// write pixels (left first)
				for( dword x = 0;  x < width;  ++x )
				{
					const dword row    = (orderingFlags & IS_TOP_FIRST) != 0 ?
						height - 1 - y : y;
					const dword offset = ((row * width) + x) * 3;

					// write channels (R then G then B)
					for( dword c = 0;  c < 3;  ++c )
					{
						const dword i = (orderingFlags & IS_BGR) != 0 ? 2 - c : c;

						// 24 bit
						if( !is48Bit )
						{
							const ubyte* pTriple =
								static_cast<const ubyte*>(pTriples) + offset;

							out << ubyte(pTriple[i]);
						}
						// 48 bit
						else
						{
							const uword* pTriple =
								static_cast<const uword*>(pTriples) + offset;

							// msb first
							out << ubyte((pTriple[i] & 0xFF00) >> 8);
							out << ubyte( pTriple[i] & 0xFF);
						}
					}
				}
			}
		}
	}
	catch( ... )
	{
		out.exceptions( originalExceptionFlags );

		throw STREAM_EXCEPTION_MESSAGE;
	}

	out.exceptions( originalExceptionFlags );
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <fstream>
#include <sstream>


namespace p3tonemapper_format
{
namespace ppm
{
	using namespace hxa7241;


bool test_ppm
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_ppm ]\n\n";


	/*// write image files -- for manual inspection
	{
		for( dword i = 0;  i < 2;  ++i )
		{
			// make pixels
			const dword wpow    = 8;   // >= 2
			const dword width   = 1 << wpow;
			const dword height  = width / 2;
			const bool  is48Bit = i & 1;

			ubyte* pPixels = new ubyte[ (width * height * 3) << dword(is48Bit) ];

			for( dword h = height;  h-- > 0; )
			{
				for( dword w = width;  w-- > 0; )
				{
					const dword column = (w >> (wpow - 2)) & 0x03;
					const float row    = float(h) / float(height - 1);
					const dword offset = (w + (h * width)) * 3;

					for( dword p = 3;  p-- > 0; )
					{
						if( !is48Bit )
						{
							const ubyte value = ubyte( row * float(ubyte(-1)) );

							pPixels[ offset + p ] =
								((p == column) | (3 == column)) ? value : 0;
						}
						else
						{
							const uword value = uword( row * float(uword(-1)) );

							reinterpret_cast<uword*>(pPixels)[ offset + p ] =
								((p == column) | (3 == column)) ? value : 0;
						}
					}
				}
			}

			// write image
			std::ofstream outf( !is48Bit ? "zzztest24.ppm" : "zzztest48.ppm",
				std::ofstream::binary );

			try
			{
				p3tonemapper_format::ppm::write(
					width, height, is48Bit, 0, pPixels, outf );

				if( pOut && isVerbose ) *pOut << "wrote image to " <<
					(!is48Bit ? "zzztest24.ppm" : "zzztest48.ppm") << "\n";
			}
			catch( ... )
			{
				delete[] pPixels;

				throw;
			}

			delete[] pPixels;
		}

		if( pOut && isVerbose ) *pOut << "\n";
	}*/


	// 24 bit write
	{
		const byte pixels[] = { byte(-1), byte(0), byte(0),   byte(0), byte(-1), byte(0),   byte(0), byte(0), byte(-1),
										byte(10), byte(10), byte(10),   byte(100), byte(100), byte(100),   byte(-56), byte(-56), byte(-56) };

		//std::ofstream out24f( "zzztest24.ppm", std::ofstream::binary );
		//p3tonemapper_format::ppm::write( 3, 2, false, 0, pixels, out24f );

		std::ostringstream out24( std::ostringstream::binary );

		p3tonemapper_format::ppm::write( 3, 2, false, 0, pixels, out24 );

		// should result in a file consisting of these hex values:
		//
		// 50 36 0A
		// 23 20 68 74 74 70 3A 2F 2F 77 77 77 2E 68 78 61 37 32 34 31 2E 6F 72 67 2F 0A
		// 0A
		// 33 20 32 0A
		// 32 35 35 0A
		// 0A 0A 0A 64 64 64 C8 C8 C8 FF 00 00 00 FF 00 00 00 FF
		const udword correct[] = {
			0x50, 0x36, 0x0A,
			0x23, 0x20, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x68, 0x78, 0x61, 0x37, 0x32, 0x34, 0x31, 0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x0A,
			0x0A,
			0x33, 0x20, 0x32, 0x0A,
			0x32, 0x35, 0x35, 0x0A,
			0x0A, 0x0A, 0x0A, 0x64, 0x64, 0x64, 0xC8, 0xC8, 0xC8, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF };

		bool isFail = false;
		std::string buf( out24.str() );
		if( buf.size() == sizeof(correct)/sizeof(correct[0]) )
		{
			if( pOut && isVerbose ) std::hex( *pOut );
			if( pOut && isVerbose ) std::uppercase( *pOut );
			for( dword i = 0;  i < dword(sizeof(correct)/sizeof(correct[0]));  ++i )
			{
				const udword value = udword(ubyte(buf[i]));
				if( pOut && isVerbose ) *pOut << (value < 16 ? "0" : "") << value <<
					((i & 0x1F) == 0x1F ? "\n" : " ");
				isFail |= (correct[i] != value);
			}
			if( pOut && isVerbose ) std::nouppercase( *pOut );
			if( pOut && isVerbose ) std::dec( *pOut );
			if( pOut && isVerbose ) *pOut << "\n";
		}
		else
		{
			isFail |= true;
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "24bit write : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	// 48 bit write
	{
		const word pixelsW[] = { word(-256), word(0), word(0),   word(0), word(-256), word(0),   word(0), word(0), word(-256),
										 word(0x0A00), word(0x0A00), word(0x0A00),   word(0x6400), word(0x6400), word(0x6400),   word(-14336), word(-14336), word(-14336) };

		//std::ofstream out48f( "zzztest48.ppm", std::ofstream::binary );
		//p3tonemapper_format::ppm::write( 3, 2, true, IS_TOP_FIRST, pixelsW, out48f );

		std::ostringstream out48( std::ostringstream::binary );

		p3tonemapper_format::ppm::write( 3, 2, true, IS_TOP_FIRST, pixelsW, out48 );

		// should result in a file consisting of these hex values:
		//
		// 50 36 0A
		// 23 20 68 74 74 70 3A 2F 2F 77 77 77 2E 68 78 61 37 32 34 31 2E 6F 72 67 2F 0A
		// 0A
		// 33 20 32 0A
		// 36 35 35 33 35 0A
		// 0A 00 0A 00 0A 00 64 00 64 00 64 00 C8 00 C8 00 C8 00 FF 00 00 00 00 00 00 00 FF 00 00 00 00 00 00 00 FF 00
		const udword correct[] = {
			0x50, 0x36, 0x0A,
			0x23, 0x20, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x68, 0x78, 0x61, 0x37, 0x32, 0x34, 0x31, 0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x0A,
			0x0A,
			0x33, 0x20, 0x32, 0x0A,
			0x36, 0x35, 0x35, 0x33, 0x35, 0x0A,
			0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x64, 0x00, 0x64, 0x00, 0x64, 0x00, 0xC8, 0x00, 0xC8, 0x00, 0xC8, 0x00 };

		bool isFail = false;
		std::string buf( out48.str() );
		if( buf.size() == sizeof(correct)/sizeof(correct[0]) )
		{
			if( pOut && isVerbose ) std::hex( *pOut );
			if( pOut && isVerbose ) std::uppercase( *pOut );
			for( dword i = 0;  i < dword(sizeof(correct)/sizeof(correct[0]));  ++i )
			{
				const udword value = udword(ubyte(buf[i]));
				if( pOut && isVerbose ) *pOut << (value < 16 ? "0" : "") << value <<
					((i & 0x1F) == 0x1F ? "\n" : " ");
				isFail |= (correct[i] != value);
			}
			if( pOut && isVerbose ) std::nouppercase( *pOut );
			if( pOut && isVerbose ) std::dec( *pOut );
			if( pOut && isVerbose ) *pOut << "\n";
		}
		else
		{
			isFail |= true;
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "48bit write : " <<
			(!isFail ? "--- succeeded" : "*** failed") << "\n\n";
		isOk &= !isFail;
	}


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace
}//namespace


#endif//TESTING
