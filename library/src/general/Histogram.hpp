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


#ifndef Histogram_h
#define Histogram_h


#include "Array.hpp"
#include "Interval.hpp"




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/**
 * A basic histogram -- storing counts in uniform bins along a real
 * interval.<br/><br/>
 *
 * a refactoring extract -- so not a very well-rounded class yet.
 *
 * Allows negative bin values.
 * Probably allows negative intervals (depending on Interval).
 * If the interval is empty, addToBin only uses the first bin.
 *
 * @see
 * Array<>
 * Interval
 *
 * @invariants
 * bins_m length > 0
 * bins_m length <= getMaxSize()
 */
class Histogram
{
/// standard object services ---------------------------------------------------
public:
	         Histogram();
	         Histogram( dword           noOfBins,
	                    const Interval& xAxis );

	virtual ~Histogram();
	         Histogram( const Histogram& );
	Histogram& operator=( const Histogram& );


/// commands -------------------------------------------------------------------
	virtual void  setSize( dword noOfBins );
	virtual void  setXAxis( const Interval& );

	virtual void  setAllBins( float );
	virtual void  addToBin( float position,
	                        float increment );

	virtual Array<float>& getBins();


/// queries --------------------------------------------------------------------
	virtual dword    getSize()                                             const;
	virtual Interval getXAxis()                                            const;
	virtual float    getBinWidth()                                         const;

	virtual float    getTotal()                                            const;

	virtual const Array<float>& getBins()                                  const;

	static  dword    getMaxSize();


/// fields ---------------------------------------------------------------------
private:
	Array<float> bins_m;
	Interval     xAxis_m;
};


}//namespace




#endif//Histogram_h
