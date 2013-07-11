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


#ifndef rgbe_h
#define rgbe_h


#include <iosfwd>




#include "p3tonemapper_format.hpp"
namespace p3tonemapper_format
{
	using std::istream;
	using std::ostream;


/**
 * IO for the Radiance RGBE format.<br/><br/>
 *
 * <cite>the radiance fileformats doc:
 * http://radsite.lbl.gov/radiance/refer/filefmts.pdf</cite><br/>
 * <cite>'Real Pixels', Graphics Gems 2, AP 91; Ward.</cite>
 *
 * @implementation
 * could be made more robust in two ways:
 * * fail partially instead of aborting all (simple):
 *   have an alternative top function that still catches exceptions but then
 *   returns a failure code (instead of deallocating image and rethrowing).
 * * run-length row isolation (moderate):
 *   detect when a row-run doesn't fit, then recover by finding the next
 *   row-leader and continuing.
 */
namespace rgbe
{
	/**
	 * Read stream of image.<br/><br/>
	 *
	 * Does not condition output values -- you get what is in file, valid or not.
	 * <br/><br/>
	 *
	 * @pPrimaries8  chromaticities of RGB and white:
	 *               { rx, ry, gx, gy, bx, by, wx, wy }.
	 *               all 0 if not present, and should assume sRGB
	 * @exposure  divide pixel values by this to get cd/m^2, is 0 if not present.
	 *
	 * @exceptions throws char[] message and allocation exceptions
	 */
	void  read
	(
		istream& inBytes,
		bool     isInvert,
		dword&   width,
		dword&   height,
		float*   pPrimaries8,
		float&   exposure,
		float*&  pRgbTriples
	);


//	void  write
//	(
//		dword        width,
//		dword        height,
//		bool         isInvert,
//		const float* pPrimaries8,
//		float        exposure,
//		const float* pRgbTriples,
//		ostream&     outBytes
//	);
}


}//namespace




#endif//rgbe_h
