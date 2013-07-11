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


#ifndef exr_h
#define exr_h




#include "p3tonemapper_format.hpp"
namespace p3tonemapper_format
{


/**
 * Basic IO for the OpenEXR format.<br/><br/>
 *
 * This implementation requires the OpenEXR dynamic libraries when run.
 * <br/><br/>
 *
 * <cite>http://www.openexr.com</cite>
 */
namespace exr
{
	enum EOrderingFlags
	{
		IS_LOW_TOP   = 1,
		IS_BGR       = 2
	};


	/**
	 * Read EXR image.<br/><br/>
	 *
	 * @exrLibraryPathName if path not included then a standard system search
	 *                     strategy is used.
	 * @pPrimaries8  chromaticities of RGB and white:
	 *               { rx, ry, gx, gy, bx, by, wx, wy }.
	 *               all 0 if not present.
	 * @scalingToGetCdm2   scaling needed to make cd/m^2, 0 if not present
	 *
	 * @exceptions throws allocation and char[] message exceptions
	 */
	void  read
	(
		const char exrLibraryPathName[],
		const char filePathName[],
		dword      orderingFlags,
		dword&     width,
		dword&     height,
		float*     pPrimaries8,
		float&     scalingToGetCdm2,
		float*&    pTriples
	);


//	void  write
//	(
//		const char   exrLibraryPathName[],
//		dword        width,
//		dword        height,
//		const float* pPrimaries8,
//		float        scalingToGetCdm2,
//		dword        orderingFlags,
//		const float* pTriples,
//		const char   filePathName[],
//	);
}


}//namespace




#endif//exr_h
