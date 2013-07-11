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
#include <string>
#include <vector>
//#include <setjmp.h>

#include "png.h"

#include "StringConstants.hpp"
#include "DynamicLibraryInterface.hpp"

#include "png.hpp"   // own header is included last


using namespace p3tonemapper_format;




/// constants ------------------------------------------------------------------
static const char PNG_INIT_FAIL_MESSAGE[] =
	"PNG library failed to initialize";
static const char PNG_EXCEPTION_MESSAGE[] =
	"PNG library failure";
static const char STREAM_EXCEPTION_MESSAGE[] =
	"stream write failure, in PNG write";

static const char LIB_PATHNAME_DEFAULT[] =
#ifdef _PLATFORM_WIN
	"libpng13.dll";
#elif _PLATFORM_LINUX
	"libpng.so.3";
#else
	"";
#endif


/// globals
static void* library_g = 0;




/// libpng callbacks interface -------------------------------------------------
static void writePngData
(
	png_structp pPngObj,
	png_bytep   pData,
	png_size_t  length
);

static void flushPngData
(
	png_structp pPngObj
);

//static void reOrderBytes
//(
//	png_structp   pPngObj,
//	png_row_infop pRowInfo,
//	png_bytep     pData
//);

static void attendToPngError
(
	png_structp     pPngObj,
	png_const_charp pErrorMsg
);

static void attendToPngWarning
(
	png_structp     pPngObj,
	png_const_charp pWarningMsg
);




/// ----------------------------------------------------------------------------
void p3tonemapper_format::png::write
(
	const char   pngLibraryPathName[],
	dword        width,
	dword        height,
	const float* pPrimaries42,
	const float  gamma,
	const bool   is48Bit,
	const dword  orderingFlags,
	const void*  pTriples,
	ostream&     out
)
{
	// load library
	{
		// use default name if needed
		const char* pPngLibraryPathName = pngLibraryPathName;
		if( (0 == pngLibraryPathName) || (0 == pngLibraryPathName[0]) )
		{
			pPngLibraryPathName = LIB_PATHNAME_DEFAULT;
		}

		hxa7241_general::loadLibrary( pPngLibraryPathName, library_g );
	}

	// enable stream exceptions
	std::ios_base::iostate originalExceptionFlags = out.exceptions();
	out.exceptions( ostream::goodbit );

	png_structp pPngObj  = 0;
	png_infop   pPngInfo = 0;
	std::string pngErrorMsg;

	try
	{
		width  = (width  >= 0) ? width  : 0;
		height = (height >= 0) ? height : 0;

		// create basic png objects
		{
			pPngObj = ::png_create_write_struct( PNG_LIBPNG_VER_STRING,
				&pngErrorMsg, attendToPngError, attendToPngWarning );
			if( 0 == pPngObj )
			{
				throw PNG_INIT_FAIL_MESSAGE;
			}

			pPngInfo = ::png_create_info_struct( pPngObj );
			if( 0 == pPngInfo)
			{
				throw PNG_INIT_FAIL_MESSAGE;
			}
		}

		// set the target for the png 'exception' jump
		if( ::setjmp( pPngObj->jmpbuf ) )
		{
			throw PNG_EXCEPTION_MESSAGE;
		}

		// set some general callbacks and options
		{
			::png_set_write_fn( pPngObj, &out, writePngData, flushPngData );
			//::png_set_write_status_fn( pPngObj, attendToPngRowWritten );
				//void attendToPngRowWritten( png_ptr, png_uint_32 row, int pass );

			::png_set_filter( pPngObj, 0, PNG_ALL_FILTERS );
			::png_set_compression_level( pPngObj, 9 );//Z_BEST_COMPRESSION );
		}

		// set some specific chunks
		{
			::png_set_IHDR( pPngObj, pPngInfo,
				width, height, 8 << dword(is48Bit),
				PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

			if( 0 != pPrimaries42 )
			{
				::png_set_cHRM( pPngObj, pPngInfo,
					pPrimaries42[6], pPrimaries42[7],
					pPrimaries42[0], pPrimaries42[1],
					pPrimaries42[2], pPrimaries42[3],
					pPrimaries42[4], pPrimaries42[5] );
			}

			if( 0.0f != gamma )
			{
				::png_set_gAMA( pPngObj, pPngInfo, gamma );
			}

			png_text texts[] = { {
					PNG_TEXT_COMPRESSION_NONE,
					const_cast<char*>("Software"),
					const_cast<char*>(hxa7241_general::HXA7241_URI()),
					0 } };
			::png_set_text( pPngObj, pPngInfo, texts, 1 );
		}

		// write before-image stuff
		::png_write_info( pPngObj, pPngInfo );

		// set pixel byte ordering
		{
			if( 0 != (orderingFlags & IS_BGR) )
			{
				::png_set_bgr( pPngObj );
			}

			if( is48Bit & (0 == (orderingFlags & IS_LO_ENDIAN)) )
			{
				::png_set_swap( pPngObj );
			}

			//::png_set_write_user_transform_fn( pPngObj, reOrderBytes );
		}

		// write pixels
		{
			std::vector<void*> rowPtrs( height );
			for( dword i = height;  i-- > 0; )
			{
				const dword row = (0 != (orderingFlags & IS_TOP_FIRST)) ?
					i : (height - 1) - i;
				const dword offset = row * (width * (3 << dword(is48Bit)));
				rowPtrs[i] =
					static_cast<ubyte*>(const_cast<void*>(pTriples)) + offset;
			}

			::png_write_image(
				pPngObj, reinterpret_cast<png_bytepp>(&(rowPtrs[0])) );
		}

		// finish writing
		::png_write_end( pPngObj, 0 );

		// delete basic png objects
		::png_destroy_write_struct( &pPngObj, &pPngInfo );

		// free library
		hxa7241_general::freeLibrary( library_g );
	}
	catch( ... )
	{
		if( 0 != pPngObj )
		{
			::png_destroy_write_struct( &pPngObj, &pPngInfo );
		}

		hxa7241_general::freeLibrary( library_g );

		out.exceptions( originalExceptionFlags );

		throw;
	}

	out.exceptions( originalExceptionFlags );
}




/// libpng callbacks -----------------------------------------------------------
void writePngData
(
	png_structp pPngObj,
	png_bytep   pData,
	png_size_t  length
)
{
	ostream* pOstream =
		reinterpret_cast<ostream*>( ::png_get_io_ptr( pPngObj ) );

	pOstream->write( reinterpret_cast<const char*>(pData), length );
	if( pOstream->fail() )
	{
		throw STREAM_EXCEPTION_MESSAGE;
	}
}


void flushPngData
(
	png_structp pPngObj
)
{
	ostream* pOstream =
		reinterpret_cast<ostream*>( ::png_get_io_ptr( pPngObj ) );

	pOstream->flush();
	if( pOstream->fail() )
	{
		throw STREAM_EXCEPTION_MESSAGE;
	}
}


//void reOrderBytes
//(
//	png_structp   pPngObj,
//	png_row_infop pRowInfo,
//	png_bytep     pData
//)
//{
//}


void attendToPngError
(
	png_structp     pPngObj,
	png_const_charp pErrorMsg
)
{
	std::string* pString =
		reinterpret_cast<std::string*>( ::png_get_error_ptr(pPngObj) );
	*pString = pErrorMsg;

	::longjmp( pPngObj->jmpbuf, 1 );
}


void attendToPngWarning
(
	png_structp,
	png_const_charp //pWarningMsg
)
{
//	using hxa7241_general::Logger;
//	Logger() << Logger::EXC1 << PngWriter::CLASS_NAME() << " : " << pWarningMsg;
}




/// libpng dynamic library forwarders ------------------------------------------

// (generating these automatically with some kind of macro or template might be
// good...)
// (but theyre done now, and what could be easier than doing nothing and using
// them as they are ?)

png_structp  png_create_write_struct
(
   png_const_charp user_png_ver,
   png_voidp       error_ptr,
   png_error_ptr   error_fn,
   png_error_ptr   warn_fn
)
{
	typedef png_structp (*PFunction)(
		png_const_charp,
		png_voidp,
		png_error_ptr,
		png_error_ptr
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_create_write_struct" ) );

	return (function)(
		user_png_ver,
		error_ptr,
		error_fn,
		warn_fn
	);
}


png_infop  png_create_info_struct
(
   png_structp png_ptr
)
{
	typedef png_infop (*PFunction)(
		png_structp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_create_info_struct" ) );

	return (function)(
		png_ptr
	);
}


void  png_set_write_fn
(
	png_structp   png_ptr,
	png_voidp     io_ptr,
	png_rw_ptr    write_data_fn,
	png_flush_ptr output_flush_fn
)
{
	typedef void (*PFunction)(
		png_structp,
		png_voidp,
		png_rw_ptr,
		png_flush_ptr
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_write_fn" ) );

	return (function)(
		png_ptr,
		io_ptr,
		write_data_fn,
		output_flush_fn
	);
}


//void  png_set_write_status_fn
//(
//	png_structp          png_ptr,
//	png_write_status_ptr write_row_fn
//)
//{
//}


void  png_set_filter
(
	png_structp png_ptr,
	int         method,
	int         filters
)
{
	typedef void (*PFunction)(
		png_structp,
		int,
		int
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_filter" ) );

	return (function)(
		png_ptr,
		method,
		filters
	);
}


void  png_set_compression_level
(
	png_structp png_ptr,
	int         level
)
{
	typedef void (*PFunction)(
		png_structp,
		int
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_compression_level" ) );

	return (function)(
		png_ptr,
		level
	);
}


void  png_set_IHDR
(
	png_structp png_ptr,
	png_infop   info_ptr,
	png_uint_32 width,
	png_uint_32 height,
	int         bit_depth,
	int         color_type,
	int         interlace_method,
	int         compression_method,
	int         filter_method
)
{
	typedef void (*PFunction)(
		png_structp,
		png_infop,
		png_uint_32,
		png_uint_32,
		int,
		int,
		int,
		int,
		int
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_IHDR" ) );

	return (function)(
		png_ptr,
		info_ptr,
		width,
		height,
		bit_depth,
		color_type,
		interlace_method,
		compression_method,
		filter_method
	);
}


void  png_set_cHRM
(
	png_structp png_ptr,
	png_infop   info_ptr,
	double      white_x,
	double      white_y,
	double      red_x,
	double      red_y,
	double      green_x,
	double      green_y,
	double      blue_x,
	double      blue_y
)
{
	typedef void (*PFunction)(
		png_structp,
		png_infop,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_cHRM" ) );

	return (function)(
		png_ptr,
		info_ptr,
		white_x,
		white_y,
		red_x,
		red_y,
		green_x,
		green_y,
		blue_x,
		blue_y
	);
}


void  png_set_gAMA
(
	png_structp png_ptr,
	png_infop   info_ptr,
	double      gamma
)
{
	typedef void (*PFunction)(
		png_structp,
		png_infop,
		double
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_gAMA" ) );

	return (function)(
		png_ptr,
		info_ptr,
		gamma
	);
}


void  png_set_text
(
	png_structp png_ptr,
	png_infop   info_ptr,
	png_textp   text_ptr,
	int         num_text
)
{
	typedef void (*PFunction)(
		png_structp,
		png_infop,
		png_textp,
		int
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_text" ) );

	return (function)(
		png_ptr,
		info_ptr,
		text_ptr,
		num_text
	);
}


void  png_write_info
(
	png_structp png_ptr,
	png_infop   info_ptr
)
{
	typedef void (*PFunction)(
		png_structp,
		png_infop
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_write_info" ) );

	return (function)(
		png_ptr,
		info_ptr
	);
}


void  png_set_bgr
(
	png_structp png_ptr
)
{
	typedef void (*PFunction)(
		png_structp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_bgr" ) );

	return (function)(
		png_ptr
	);
}


void  png_set_swap
(
	png_structp png_ptr
)
{
	typedef void (*PFunction)(
		png_structp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_set_swap" ) );

	return (function)(
		png_ptr
	);
}


//void  png_set_write_user_transform_fn
//(
//	png_structp            png_ptr,
//	png_user_transform_ptr write_user_transform_fn
//)
//{
//}


void  png_write_image
(
	png_structp png_ptr,
	png_bytepp  image
)
{
	typedef void (*PFunction)(
		png_structp,
		png_bytepp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_write_image" ) );

	return (function)(
		png_ptr,
		image
	);
}


void  png_write_end
(
	png_structp png_ptr,
	png_infop   info_ptr
)
{
	typedef void (*PFunction)(
		png_structp,
		png_infop
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_write_end" ) );

	return (function)(
		png_ptr,
		info_ptr
	);
}


png_voidp  png_get_io_ptr
(
	png_structp png_ptr
)
{
	typedef png_voidp (*PFunction)(
		png_structp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_get_io_ptr" ) );

	return (function)(
		png_ptr
	);
}


png_voidp  png_get_error_ptr
(
	png_structp png_ptr
)
{
	typedef png_voidp (*PFunction)(
		png_structp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_get_error_ptr" ) );

	return (function)(
		png_ptr
	);
}


void  png_destroy_write_struct
(
	png_structpp png_ptr_ptr,
	png_infopp   info_ptr_ptr
)
{
	typedef void (*PFunction)(
		png_structpp,
		png_infopp
	);

	PFunction function = reinterpret_cast<PFunction>(
		hxa7241_general::getFunction( library_g, "png_destroy_write_struct" ) );

	return (function)(
		png_ptr_ptr,
		info_ptr_ptr
	);
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <fstream>
#include <sstream>


namespace p3tonemapper_format
{
namespace png
{
	using namespace hxa7241;


bool test_png
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_png ]\n\n";


	static const float SRGB_PRIMARIES[] =
	{
		// x      y
		0.64f, 0.33f,   // r
		0.30f, 0.60f,   // g
		0.15f, 0.06f,   // b
		0.31273127312731f, 0.32903290329033f
	};

	static const char LIB_FILE[] = "";//"temp\\libpng13.dll";


	// write image files -- for manual inspection
	/*{
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
			std::ofstream outf( !is48Bit ? "zzztest24.png" : "zzztest48.png",
				std::ofstream::binary );

			try
			{
				p3tonemapper_format::png::write( LIB_FILE, width, height,
					SRGB_PRIMARIES, 1.0f, is48Bit, 0,
					pPixels, outf );

				if( pOut && isVerbose ) *pOut << "wrote image to " <<
					(!is48Bit ? "zzztest24.png" : "zzztest48.png") << "\n";
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


	/*// convert png to text
	{
		std::ifstream in ( "temp\\zzztest48-8x4.png", std::ifstream::binary );
		std::ofstream out( "temp\\zzztest48-8x4.txt", std::ofstream::binary );
		std::hex( out );
		std::uppercase( out );

		out << "\n\n{\n";
		while( !in.eof() )
		{
			out << "\t";
			for( dword i = 8;  (i-- > 0) & !in.eof(); )
			{
				ubyte b = ubyte(in.get());
				out << "0x" << uword( b ) << ", ";
			}
			out << "\n";
		}
		out << "};\n\n";
	}*/


	// 24 bit write
	{
		// make pixels
		const dword wpow    = 3;   // >= 2
		const dword width   = 1 << wpow;
		const dword height  = width / 2;

		ubyte pixels[ width * height * 3 ];

		for( dword h = height;  h-- > 0; )
		{
			for( dword w = width;  w-- > 0; )
			{
				const dword column = (w >> (wpow - 2)) & 0x03;
				const float row    = float(h) / float(height - 1);
				const ubyte value  = ubyte( row * float(ubyte(-1)) );
				const dword offset = (w + (h * width)) * 3;

				for( dword p = 3;  p-- > 0; )
				{
					pixels[ offset + p ] =
						((p == column) | (3 == column)) ? value : 0;
				}
			}
		}

		// stream out pixels
		std::ostringstream out24( std::ostringstream::binary );

		p3tonemapper_format::png::write( LIB_FILE, width, height,
			SRGB_PRIMARIES, 1.0f, false, 0,
			pixels, out24 );


		// should result in a file consisting of these hex values:
		static const udword correct[] = {
			0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
			0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
			0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04,
			0x08, 0x02, 0x00, 0x00, 0x00, 0x3C, 0xAF, 0xE9,
			0xA7, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D,
			0x41, 0x00, 0x01, 0x86, 0xA0, 0x31, 0xE8, 0x96,
			0x5F, 0x00, 0x00, 0x00, 0x20, 0x63, 0x48, 0x52,
			0x4D, 0x00, 0x00, 0x7A, 0x29, 0x00, 0x00, 0x80,
			0x87, 0x00, 0x00, 0xFA, 0x00, 0x00, 0x00, 0x80,
			0xE8, 0x00, 0x00, 0x75, 0x30, 0x00, 0x00, 0xEA,
			0x60, 0x00, 0x00, 0x3A, 0x98, 0x00, 0x00, 0x17,
			0x70, 0xF5, 0xED, 0x2B, 0x70, 0x00, 0x00, 0x00,
			0x20, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 0x66,
			0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x68, 0x74,
			0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77,
			0x2E, 0x68, 0x78, 0x61, 0x37, 0x32, 0x34, 0x31,
			0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x19, 0xBB, 0x1B,
			0x2F, 0x00, 0x00, 0x00, 0x2A, 0x49, 0x44, 0x41,
			0x54, 0x08, 0xD7, 0x75, 0xC5, 0x31, 0x11, 0x00,
			0x20, 0x0C, 0x04, 0xB0, 0x70, 0xE0, 0xDF, 0x48,
			0xFD, 0xBC, 0x9D, 0xB2, 0x96, 0x81, 0x2C, 0x59,
			0x0D, 0xD6, 0xA8, 0x1B, 0x4E, 0x01, 0xA3, 0x2A,
			0xD8, 0x08, 0x79, 0x4B, 0xE2, 0xE7, 0x02, 0x86,
			0xE9, 0x0F, 0xE3, 0xED, 0xE2, 0xC6, 0x06, 0x00,
			0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
			0x42, 0x60, 0x82
		};

		bool isFail = false;
		std::string buf( out24.str() );
		if( buf.size() == sizeof(correct)/sizeof(correct[0]) )
		{
			if( pOut && isVerbose ) std::hex( *pOut );
			if( pOut && isVerbose ) std::uppercase( *pOut );
			for( dword i = 0;  i < dword(sizeof(correct)/sizeof(correct[0]));  ++i)
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
		// make pixels
		const dword wpow    = 3;   // >= 2
		const dword width   = 1 << wpow;
		const dword height  = width / 2;

		uword pixels[ width * height * 3 ];

		for( dword h = height;  h-- > 0; )
		{
			for( dword w = width;  w-- > 0; )
			{
				const dword column = (w >> (wpow - 2)) & 0x03;
				const float row    = float(h) / float(height - 1);
				const uword value  = uword( row * float(uword(-1)) );
				const dword offset = (w + (h * width)) * 3;

				for( dword p = 3;  p-- > 0; )
				{
					pixels[ offset + p ] =
						((p == column) | (3 == column)) ? value : 0;
				}
			}
		}

		// stream out pixels
		std::ostringstream out48( std::ostringstream::binary );

		p3tonemapper_format::png::write( LIB_FILE, width, height,
			SRGB_PRIMARIES, 1.0f, true, 0,
			pixels, out48 );


		// should result in a file consisting of these hex values:
		static const udword correct[] = {
			0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
			0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
			0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04,
			0x10, 0x02, 0x00, 0x00, 0x00, 0x6C, 0x3F, 0x35,
			0xE4, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D,
			0x41, 0x00, 0x01, 0x86, 0xA0, 0x31, 0xE8, 0x96,
			0x5F, 0x00, 0x00, 0x00, 0x20, 0x63, 0x48, 0x52,
			0x4D, 0x00, 0x00, 0x7A, 0x29, 0x00, 0x00, 0x80,
			0x87, 0x00, 0x00, 0xFA, 0x00, 0x00, 0x00, 0x80,
			0xE8, 0x00, 0x00, 0x75, 0x30, 0x00, 0x00, 0xEA,
			0x60, 0x00, 0x00, 0x3A, 0x98, 0x00, 0x00, 0x17,
			0x70, 0xF5, 0xED, 0x2B, 0x70, 0x00, 0x00, 0x00,
			0x20, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 0x66,
			0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x68, 0x74,
			0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77,
			0x2E, 0x68, 0x78, 0x61, 0x37, 0x32, 0x34, 0x31,
			0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x19, 0xBB, 0x1B,
			0x2F, 0x00, 0x00, 0x00, 0x2E, 0x49, 0x44, 0x41,
			0x54, 0x08, 0xD7, 0x63, 0xFC, 0xFF, 0x9F, 0x01,
			0x0E, 0x18, 0x19, 0xB1, 0xF3, 0xFE, 0xFF, 0x47,
			0x88, 0xB3, 0xAC, 0x5E, 0xCD, 0x80, 0x04, 0xB0,
			0xF3, 0x56, 0xAF, 0x46, 0x88, 0x33, 0x43, 0xA8,
			0x6B, 0xD7, 0x10, 0x24, 0x2E, 0x1E, 0x04, 0x30,
			0x90, 0x0A, 0x00, 0xCC, 0x55, 0x1F, 0xBD, 0x8E,
			0x0A, 0xC8, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x49,
			0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82
		};

		bool isFail = false;
		std::string buf( out48.str() );
		if( buf.size() == sizeof(correct)/sizeof(correct[0]) )
		{
			if( pOut && isVerbose ) std::hex( *pOut );
			if( pOut && isVerbose ) std::uppercase( *pOut );
			for( dword i = 0;  i < dword(sizeof(correct)/sizeof(correct[0]));  ++i)
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
