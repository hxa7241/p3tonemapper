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


#ifndef png_h
#define png_h


#include <iosfwd>




#include "p3tonemapper_format.hpp"
namespace p3tonemapper_format
{
	using std::istream;
	using std::ostream;


/**
 * Basic IO for the PNG format.<br/><br/>
 *
 * Supports these features:
 * * RGB
 * * 24 or 48 bit pixels
 * * can include primaries and gamma
 * * allows byte, pixel, and row re-ordering
 * <br/>
 *
 * This implementation requires the libpng and zlib dynamic libraries when run.
 * (from, for example, libpng-1.2.8-bin and libpng-1.2.8-dep archives)<br/><br/>
 *
 * <cite>http://www.libpng.org/</cite>
 */
namespace png
{
	enum EOrderingFlags
	{
		IS_TOP_FIRST = 1,
		IS_BGR       = 2,
		IS_LO_ENDIAN = 4
	};


	/**
	 * Write PNG image.<br/><br/>
	 *
	 * triples are bottom row first, R then G then B.
	 *
	 * @pngLibraryPathName if path not included then a standard system search
	 *                     strategy is used.
	 * @pPrimaries42       chromaticities of RGB and white:
	 *                     { rx, ry, gx, gy, bx, by, wx, wy }
	 *                     if zero, they are not written to image metadata
	 * @gamma              if zero, it is not written to image metadata
	 * @is48Bit            true for 48 bit triples, false for 24
	 * @orderingFlags      bit combination of EOrderingFlags values
	 * @pTriples           array of byte triples, or word triples if is48Bit is
	 *                     true
	 *
	 * @exceptions throws char[] message exceptions
	 */
	void  write
	(
		const char   pngLibraryPathName[],
		dword        width,
		dword        height,
		const float* pPrimaries42,
		float        gamma,
		bool         is48Bit,
		dword        orderingFlags,
		const void*  pTriples,
		ostream&     outBytes
	);
}


}//namespace




#endif//png_h
