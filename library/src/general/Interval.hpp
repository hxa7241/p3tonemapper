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


#ifndef Interval_h
#define Interval_h




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/**
 * An interval on the real line, made of two floats.<br/><br/>
 *
 * Can be negative: lower > upper.
 * Can be empty: lower == upper.
 *
 * @invariants
 * maybe lower_m could be <= upper_m ... (but it isnt currently)
 */
class Interval
{
/// standard object services ---------------------------------------------------
public:
	         Interval();
	         Interval( float lower,
	                   float upper );

	        ~Interval();
	         Interval( const Interval& );
	Interval& operator=( const Interval& );


/// commands -------------------------------------------------------------------
	        void  set( float lower,
	                   float upper );
	        void  setLower( float );
	        void  setUpper( float );

	        /**
	         * Sets the interval to the min and max of argument.
	         */
	        void  ratchetMinMax( float );


/// queries --------------------------------------------------------------------
	        void  get( float& lower,
	                   float& upper )                                      const;
	        float getLower()                                               const;
	        float getUpper()                                               const;

	        Interval getNegative()                                         const;

	        /**
	         * Upper minus lower.
	         */
	        float getRange()                                               const;
	        float getRangeAbs()                                            const;

	        /**
	         * The argument's position along the interval.
	         * doesn't check for empty interval -- may divide by zero.
	         */
	        float getInterpolant( float )                                  const;
	        /**
	         * The argument's position along the interval.
	         * returns zero for empty interval.
	         */
	        float getInterpolantSafe( float )                              const;
	        /**
	         * The argument's position along the interval.
	         * returns value clamped to [0-1], and zero for empty interval.
	         */
	        float getInterpolantClamped( float )                           const;

	        bool  isInside( float )                                        const;

	        /**
	         * +1 when range is positive, -1 when negative, 0 when empty.
	         */
	        float getSign()                                                const;
	        /**
	         * Is lower equal to upper.
	         */
	        bool  isEmpty()                                                const;
	        bool  isPositive()                                             const;
	        bool  isNegative()                                             const;

	        bool  operator==( const Interval& )                            const;
	        bool  operator!=( const Interval& )                            const;

	        /**
	         * Compare ranges.
	         */
	        bool  operator< ( const Interval& )                            const;
	        bool  operator> ( const Interval& )                            const;
	        bool  operator<=( const Interval& )                            const;
	        bool  operator>=( const Interval& )                            const;


/// constants ------------------------------------------------------------------
	static const Interval& ZERO();
	static const Interval& ZEROTOONE();
	static const Interval& UNIT();


/// fields ---------------------------------------------------------------------
private:
	float lower_m;
	float upper_m;
};








/// INLINES ///


/// standard object services ---------------------------------------------------
inline
Interval::~Interval()
{
}


/// queries --------------------------------------------------------------------
inline
float Interval::getLower() const
{
	return lower_m;
}


inline
float Interval::getUpper() const
{
	return upper_m;
}


inline
bool Interval::operator!=
(
	const Interval& other
) const
{
	return !operator==( other );
}


inline
bool Interval::operator<=
(
	const Interval& other
) const
{
	return !operator>( other );
}


inline
bool Interval::operator>=
(
	const Interval& other
) const
{
	return !operator<( other );
}


}//namespace




#endif//Interval_h
