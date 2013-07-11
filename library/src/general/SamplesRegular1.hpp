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


#ifndef SamplesRegular1_h
#define SamplesRegular1_h


#include "Array.hpp"
#include "Interval.hpp"




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/**
 * A set of sample values at regular points along a line interval.<br/><br/>
 *
 * a refactoring extract -- so not a very well-rounded class yet.
 *
 * Probably allows negative intervals (depending on Interval).

 * (not a great name, but what else?)
 *
 * @see
 * Array<>
 * Interval
 */
class SamplesRegular1
{
/// standard object services ---------------------------------------------------
public:
	         SamplesRegular1();
	         SamplesRegular1( dword           samplesCount,
	                          const Interval& xAxis );

	virtual ~SamplesRegular1();
	         SamplesRegular1( const SamplesRegular1& );
	SamplesRegular1& operator=( const SamplesRegular1& );


/// commands -------------------------------------------------------------------
	virtual void  setSize( dword noOfSamples );
	virtual void  setXAxis( const Interval& );

	virtual void  setYAxis( float scaling,
	                        float offset );
	/**
	 * Set as cumulative function made from histogram.<br/><br/>
	 * @return total of accumulation
	 */
	virtual float setCumulative( const Array<float>& histogram,
	                             const Interval&     xAxis );
	virtual float setCumulative( const Array<float>& histogram,
	                             const Interval&     xAxis,
	                             const Interval&     yAxis );
	virtual void  setAll( float );


/// queries --------------------------------------------------------------------
	virtual dword    getSize()                                             const;
	virtual Interval getXAxis()                                            const;

	/**
	 * Get interpolated value (y) between samples, for a position (x).<br/><br/>
	 * xValue clamped within x axis.
	 * If the interval is empty, the first sample is returned.
	 */
	virtual float getValue( float xValue )                                 const;

	virtual const Array<float>& getSamples()                               const;


/// fields ---------------------------------------------------------------------
private:
	Array<float> values_m;
	Interval     xAxis_m;
};


}//namespace




#endif//SamplesRegular1_h
