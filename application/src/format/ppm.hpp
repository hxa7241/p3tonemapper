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


#ifndef ppm_h
#define ppm_h


#include <iosfwd>




#include "p3tonemapper_format.hpp"
namespace p3tonemapper_format
{
	using std::istream;
	using std::ostream;


/**
 * IO for the nice-and-simple PPM format.<br/><br/>
 *
 * PPMs are uncompressed, unadorned 24 or 48 bit RGB images.
 * (just: ID, then width and height and depth, then triples)
 * Easy to use and code for, or good just to hack up a quick image IO, for
 * testing etc.<br/><br/>
 *
 * Only one image per file is implemented here.<br/><br/>
 *
 * <cite>http://netpbm.sourceforge.net/doc/ppm.html</cite>
 */
namespace ppm
{
	enum EOrderingFlags
	{
		IS_TOP_FIRST = 1,
		IS_BGR       = 2
	};

//	/**
//	 * Read PPM image.<br/><br/>
//	 *
//	 * Only first image read.<br/><br/>
//	 *
//	 * triples are bottom row first, R then G then B.
//	 */
//	void  read
//	(
//		istream& inBytes,
//		dword    orderingFlags,
//		bool     is48Bit,
//		dword&   width,
//		dword&   height,
//		void*&   pTriples
//	);


	/**
	 * Write PPM image.<br/><br/>
	 *
	 * triples are bottom row first, R then G then B.
	 *
	 * @exceptions throws char[] message exceptions
	 */
	void  write
	(
		dword       width,
		dword       height,
		bool        is48Bit,
		dword       orderingFlags,
		const void* pTriples,
		ostream&    outBytes
	);
}


}//namespace




#endif//ppm_h
