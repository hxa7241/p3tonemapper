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


#include "Clamps.hpp"




namespace hxa7241_general
{
	using namespace hxa7241;


dword clampO
(
	      dword i,
	const dword min,
	const dword max
)
{
	return clampO_( i, min, max );
}


dword clamp
(
	      dword i,
	const dword min,
	const dword max
)
{
	return clamp_( i, min, max );
}


float clamp
(
	      float f,
	const float min,
	const float max
)
{
	return clamp_( f, min, max );
}


float clamp01o
(
	float f
)
{
	return clamp01o_( f );
}


float clamp01c
(
	float f
)
{
	return clamp01c_( f );
}


}//namespace
