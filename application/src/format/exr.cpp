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


#include <vector>
#include <string>

#include "DynamicLibraryInterface.hpp"

#include "ImfCRgbaFile.h"

#include "exr.hpp"   // own header is included last


using namespace p3tonemapper_format;




/// constants ------------------------------------------------------------------
static const char OPEN_FILE_EXCEPTION_MESSAGE[] =
	"EXR open file failed";

static const char LIB_PATHNAME_DEFAULT[] =
#ifdef _PLATFORM_WIN
	"IlmImf_dll.dll";
#elif _PLATFORM_LINUX
	"libIlmImf.so.2";
#else
	"";
#endif


/// globals (well, file scope really)
static void* libraries_g[] = { 0, 0, 0, 0 };




/// read sub-procedure declarations --------------------------------------------
static void loadLibraries
(
	const char exrLibraryPathName[]
);


static void freeLibraries();


static void readHeader
(
	const ImfInputFile* pExrInFile,
	dword&              width,
	dword&              height,
	bool&               isLowTop,
	float*              pPrimaries8,
	float&              scalingToGetCdm2
);


static void readPixels
(
	ImfInputFile* pExrInFile,
	dword         width,
	dword         height,
	bool          isLowTop,
	dword         orderingFlags,
	float*&       pTriples
);




/// ----------------------------------------------------------------------------
void p3tonemapper_format::exr::read
(
	const char  exrLibraryPathName[],
	const char  filePathName[],
	const dword orderingFlags,
	dword&      width,
	dword&      height,
	float*      pPrimaries8,
	float&      scalingToGetCdm2,
	float*&     pTriples
)
{
	loadLibraries( exrLibraryPathName );

	ImfInputFile* pExrInFile = 0;
	pTriples = 0;

	try
	{
		// open file
		pExrInFile = ::ImfOpenInputFile( filePathName );
		if( 0 == pExrInFile )
		{
			//const char* ::ImfErrorMessage();
			throw OPEN_FILE_EXCEPTION_MESSAGE;
		}

		// read header
		bool isLowTop = true;
		readHeader( pExrInFile, width, height, isLowTop,
			pPrimaries8, scalingToGetCdm2 );

		// read pixels
		readPixels( pExrInFile, width, height, isLowTop, orderingFlags,
			pTriples );

		// close file
		::ImfCloseInputFile( pExrInFile );

		freeLibraries();
	}
	catch( ... )
	{
		delete[] pTriples;
		pTriples = 0;

		::ImfCloseInputFile( pExrInFile );

		freeLibraries();

		throw;
	}
}


void loadLibraries
(
	const char exrLibraryPathName[]
)
{
	// use default name if nothing given
	const char* pExrLibraryPathName = exrLibraryPathName;
	if( (0 == exrLibraryPathName) || (0 == exrLibraryPathName[0]) )
	{
		pExrLibraryPathName = LIB_PATHNAME_DEFAULT;
	}

#ifdef _PLATFORM_LINUX
	// some auxiliary ILM libs must be explicitly loaded too...
	try
	{
		// analyse lib pathname
		std::string path;
		std::string version;
		{
			// assume after-last-path-sep is name
			// extract: path, libname, libname version
			const std::string pathname( pExrLibraryPathName );

			// extract path and name
			// (path contains end separator, or is empty)
			size_t namePos = pathname.rfind( '/' );
			namePos = (std::string::npos != namePos) ? namePos + 1 : 0;
			path = pathname.substr( 0, namePos );
			const std::string name( pathname.substr( namePos ) );

			// extract version
			size_t versionPos = name.rfind( ".so" );
			versionPos += (std::string::npos != versionPos) ? 3 : 0;
			version = name.substr( versionPos );
		}

		// load auxiliary library set
		// (load in this order, store handles in reverse order)
		static const char* SUPPORT_LIBS[] = {
			"libIex.so", "libHalf.so" };//, "libImath.so" };
		for( dword i = 0;  i < 2;  ++i )
		{
			const std::string libPathName( path + SUPPORT_LIBS[i] + version );
			hxa7241_general::loadLibrary(
				libPathName.c_str(), libraries_g[3 - i] );
		}
	}
	catch( ... )
	{
		freeLibraries();
		throw;
	}
#endif

	hxa7241_general::loadLibrary( pExrLibraryPathName, libraries_g[0] );
}


void freeLibraries()
{
	// free in array forwards order
	for( udword i = 0;  i < sizeof(libraries_g)/sizeof(libraries_g[0]);  ++i )
	{
		hxa7241_general::freeLibrary( libraries_g[i] );
	}
}


void readHeader
(
	const ImfInputFile* pExrInFile,
	dword&              width,
	dword&              height,
	bool&               isLowTop,
	float*              pPrimaries8,
	float&              scalingToGetCdm2
)
{
	// get header
	const ImfHeader* pExrHeader = ::ImfInputHeader( pExrInFile );
	if( 0 == pExrHeader )
	{
		//const char* ::ImfErrorMessage();
		throw OPEN_FILE_EXCEPTION_MESSAGE;
	}

	// get dimensions
	{
		int xMin;
		int xMax;
		int yMin;
		int yMax;
		::ImfHeaderDataWindow( pExrHeader, &xMin, &yMin, &xMax, &yMax );
		width  = xMax - xMin + 1;
		height = yMax - yMin + 1;

		width  = (width  >= 0) ? width  : 0;
		height = (height >= 0) ? height : 0;
	}

	// get some attributes
	{
		// row order
		isLowTop = ::ImfHeaderLineOrder( pExrHeader ) == IMF_INCREASING_Y;

		// pixel value scaling
		// whiteLuminance
		// cd/m^2 of white
		// and 'white' is rgb(1,1,1)
		// so, equals scalingToGetCdm2
		scalingToGetCdm2 = 0.0f;
		if( 0 == ::ImfHeaderFloatAttribute(
			pExrHeader, "whiteLuminance", &scalingToGetCdm2 ) )
		{
			scalingToGetCdm2 = 0.0f;
		}

		// color space
		// (not possible)
		{
//				if( 0 == ImfHeader__Attribute(
//					pExrHeader, "chromaticities",  );
			{
				for( dword i = 8;  i-- > 0; )
				{
					pPrimaries8[i] = 0.0f;
				}
			}
		}
	}
}


void readPixels
(
	ImfInputFile* pExrInFile,
	const dword   width,
	const dword   height,
	const bool    isLowTop,
	const dword   orderingFlags,
	float*&       pTriples
)
{
	// allocate storage and buffer
	pTriples = new float[ width * height * 3 ];
	std::vector<ImfRgba> exrLine( width );

	// step thru rows
	for( dword y = 0;  y < height;  ++y )
	{
		// read into line buffer (subtracting from pointer? wtf???)
		::ImfInputSetFrameBuffer(
			pExrInFile, &(exrLine[0]) - (y * width), 1, width );
		::ImfInputReadPixels( pExrInFile, y, y );

		const dword row = ((orderingFlags & exr::IS_LOW_TOP) != 0) ^ isLowTop ?
			height - y - 1 : y;
		float* pPixel = pTriples + (row * width * 3);

		// step thru pixels
		for( dword x = 0;  x < width;  ++x )
		{
			ImfHalf rgb[3] = { exrLine[x].r, exrLine[x].g, exrLine[x].b };

			// convert channels
			for( dword c = 0;  c < 3;  ++c )
			{
				const dword i = ((orderingFlags & exr::IS_BGR) != 0) ? 2 - c : c;
				pPixel[x * 3 + c] = ::ImfHalfToFloat( rgb[ i ] );
				//*(pPixel++) = ::ImfHalfToFloat( rgb[ i ] );
			}
		}
	}
}




/// exr dynamic library forwarders ---------------------------------------------
ImfInputFile* ImfOpenInputFile
(
	const char name[]
)
{
	typedef ImfInputFile* (*PFunction)(
		const char[]
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfOpenInputFile" ) );

	return (function)(
		name
	);
}


int ImfCloseInputFile
(
	ImfInputFile* pIn
)
{
	typedef int (*PFunction)(
		ImfInputFile*
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfCloseInputFile" ) );

	return (function)(
		pIn
	);
}


const ImfHeader* ImfInputHeader
(
	const ImfInputFile* pIn
)
{
	typedef const ImfHeader* (*PFunction)(
		const ImfInputFile*
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfInputHeader" ) );

	return (function)(
		pIn
	);
}


int ImfHeaderLineOrder
(
	const ImfHeader* pHdr
)
{
	typedef int (*PFunction)(
		const ImfHeader*
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfHeaderLineOrder" ) );

	return (function)(
		pHdr
	);
}


int ImfHeaderFloatAttribute
(
	const ImfHeader* pHdr,
	const char       name[],
	float*           pValue
)
{
	typedef int (*PFunction)(
		const ImfHeader*,
		const char[],
		float*
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfHeaderFloatAttribute" ) );

	return (function)(
		pHdr,
		name,
		pValue
	);
}


void ImfHeaderDataWindow
(
	const ImfHeader* pHdr,
	int*             xMin,
	int*             yMin,
	int*             xMax,
	int*             yMax
)
{
	typedef void (*PFunction)(
		const ImfHeader*,
		int*,
		int*,
		int*,
		int*
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfHeaderDataWindow" ) );

	(function)(
		pHdr,
		xMin,
		yMin,
		xMax,
		yMax
	);
}


int ImfInputSetFrameBuffer
(
	ImfInputFile* pIn,
	ImfRgba*      pBase,
	size_t        xStride,
	size_t        yStride
)
{
	typedef int (*PFunction)(
		ImfInputFile*,
		ImfRgba*,
		size_t,
		size_t
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfInputSetFrameBuffer" ) );

	return (function)(
		pIn,
		pBase,
		xStride,
		yStride
	);
}


int ImfInputReadPixels
(
	ImfInputFile* pIn,
	int           scanLine1,
	int           scanLine2
)
{
	typedef int (*PFunction)(
		ImfInputFile*,
		int,
		int
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfInputReadPixels" ) );

	return (function)(
		pIn,
		scanLine1,
		scanLine2
	);
}


float ImfHalfToFloat
(
	ImfHalf h
)
{
	typedef float (*PFunction)(
		ImfHalf
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfHalfToFloat" ) );

	return (function)(
		h
	);
}


const char* ImfErrorMessage()
{
	typedef const char* (*PFunction)();

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( libraries_g[0], "ImfErrorMessage" ) );

	return (function)();
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <fstream>
#include <sstream>

#include "png.hpp"


namespace p3tonemapper_format
{
namespace exr
{
	using namespace hxa7241;


bool test_exr
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_exr ]\n\n";


	static const char LIB_FILE[] = "";//"temp\\IlmImf_dll.dll";


	// manual inspection test
	{
		dword  width;
		dword  height;
		float  pPrimaries8[8];
		float  scalingToGetCdm2;
		float* pTriples = 0;
		try
		{
			p3tonemapper_format::exr::read(
				LIB_FILE,
				"zzztest.exr",
				0,
				width,
				height,
				pPrimaries8,
				scalingToGetCdm2,
				pTriples
			);

			if( pOut && isVerbose ) *pOut << "width " << width << "\n";
			if( pOut && isVerbose ) *pOut << "height " << height << "\n";

			if( pOut && isVerbose ) *pOut << "primaries ";
			for( dword i = 0;  i < 8;  ++i )
			{
				if( pOut && isVerbose ) *pOut << pPrimaries8[i] << " ";
			}
			if( pOut && isVerbose ) *pOut << "\n";

			if( pOut && isVerbose ) *pOut << "scalingToGetCdm2 " <<
				scalingToGetCdm2 << "\n";
			if( pOut && isVerbose ) *pOut << "pTriples " << pTriples << "\n";

			float min[3] = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
			float max[3] = { FLOAT_MIN_NEG, FLOAT_MIN_NEG, FLOAT_MIN_NEG };
			for( dword i = 0;  i < (width * height);  ++i )
			{
				for( dword c = 0;  c < 3;  ++c )
				{
					const float channel = pTriples[ i * 3 + c ];
					min[c] = (channel < min[c]) ? channel : min[c];
					max[c] = (channel > max[c]) ? channel : max[c];
				}
			}
			if( pOut && isVerbose ) *pOut << "min " << min[0] << " " << min[1] <<
				" " << min[2] << "\n";
			if( pOut && isVerbose ) *pOut << "max " << max[0] << " " << max[1] <<
				" " << max[2] << "\n";

			if( pOut && isVerbose ) *pOut << "\n";


			{
				if( pOut ) pOut->flush();

				std::vector<ubyte> pixels( width * height * 3 );
				const float scale  = 1.0f;
				const float offset = 0.0f;
				for( dword i = 0;  i < (width * height);  ++i )
				{
					for( dword c = 3;  c-- > 0; )
					{
						const float ch   = (pTriples[(i * 3) + c] * scale) + offset;
						const float ch01 = (ch < 0.0f) ? 0.0f : (
							(ch >= 1.0f) ? FLOAT_ALMOST_ONE : ch);

						pixels[(i * 3) + c] = ubyte( dword(ch01 * 256.0f) );
					}
				}

				// write image
				std::ofstream outf( "zzztest.png", std::ofstream::binary );

				p3tonemapper_format::png::write(
					"",
					width, height,
					pPrimaries8, 0.0f, false, 0,
					&(pixels[0]), outf );

				if( pOut && isVerbose ) *pOut << "wrote image to zzztest.png" <<
					"\n";
			}


		}
		catch( const std::exception& e )
		{
			if( pOut && isVerbose ) *pOut << "exception: " << e.what() << "\n";
		}
		catch( const char pMessage[] )
		{
			if( pOut && isVerbose ) *pOut << "exception: " << pMessage << "\n";
		}
		catch( ... )
		{
			if( pOut && isVerbose ) *pOut << "unannotated exception\n";
		}

		delete[] pTriples;

		if( pOut && isVerbose ) *pOut << "\n";
	}

	if( pOut && isVerbose ) *pOut << "needs manual inspection\n";
	if( pOut && isVerbose ) *pOut << "\n";


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace
}//namespace


#endif//TESTING
