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


#ifndef Vector3f_h
#define Vector3f_h


#include <iosfwd>

#include "hxa7241_general.hpp"




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{


/**
 * Yes, its the 3D vector class! much as you would expect.<br/><br/>
 *
 * If you write some 3D graphics software, then you MUST write your own vector
 * class - it is the law. So here is mine.
 */
class Vector3f
{
/// standard object services ---------------------------------------------------
public:
	                  Vector3f();
	                  Vector3f( float x,
	                            float y,
	                            float z );
	explicit          Vector3f( const float xyz[3] );

	                 ~Vector3f();
	                  Vector3f( const Vector3f& );
	        Vector3f& operator=( const Vector3f& );


	/// streaming
	friend  std::ostream& operator<<( std::ostream&,
	                                  const Vector3f& );
	friend  std::istream& operator>>( std::istream&,
	                                  Vector3f& );


/// access ---------------------------------------------------------------------
	        Vector3f& setXYZ( float x,
	                          float y,
	                          float z );
	        Vector3f& setXYZ( const float xyz[3] );

	        void      getXYZ( float& x,
	                          float& y,
	                          float& z )                                   const;
	        void      getXYZ( float xyz[3] )                               const;

	        float     getX()                                               const;
	        float     getY()                                               const;
	        float     getZ()                                               const;


/// arithmetic -----------------------------------------------------------------
	        float     sum()                                                const;
	        float     average()                                            const;
	        float     smallest()                                           const;
	        float     largest()                                            const;

	        float     length()                                             const;
	        float     dot( const Vector3f& )                               const;
	        float     distance( const Vector3f& )                          const;

	        Vector3f  operator-()                                          const;
	        Vector3f& negate();

	        Vector3f  abs()                                                const;
	        Vector3f& absEq();

	        Vector3f  unitize()                                            const;
	        Vector3f& unitizeEq();

	        Vector3f  cross( const Vector3f& )                             const;
	        Vector3f& crossEq( const Vector3f& );

	        Vector3f  operator+ ( const Vector3f& )                        const;
	        Vector3f& operator+=( const Vector3f& );

	        Vector3f  operator- ( const Vector3f& )                        const;
	        Vector3f& operator-=( const Vector3f& );

	        Vector3f  operator* ( const Vector3f& )                        const;
	        Vector3f& operator*=( const Vector3f& );

	        Vector3f  operator/ ( const Vector3f& )                        const;
	        Vector3f& operator/=( const Vector3f& );

	        Vector3f  operator* ( float )                                  const;
	        Vector3f& operator*=( float );

	        Vector3f  operator/ ( float )                                  const;
	        Vector3f& operator/=( float );

	        Vector3f  operator^ ( float )                                  const;
	        Vector3f& operator^=( float );

	        Vector3f  operator^ ( const Vector3f& )                        const;
	        Vector3f& operator^=( const Vector3f& );

	friend  Vector3f  operator*( float, const Vector3f& );
	friend  Vector3f  operator/( float, const Vector3f& );


/// logical --------------------------------------------------------------------
	        bool      operator==( const Vector3f& )                        const;
	        bool      operator!=( const Vector3f& )                        const;
	        bool      isZero()                                             const;

	        Vector3f  sign   ()                                            const;
	        Vector3f  compare( const Vector3f& )                           const;

	        // returning vectors of float(bool)
	        Vector3f  equal  ( const Vector3f& )                           const;

	        Vector3f  operator> ( const Vector3f& )                        const;
	        Vector3f  operator>=( const Vector3f& )                        const;

	        Vector3f  operator< ( const Vector3f& )                        const;
	        Vector3f  operator<=( const Vector3f& )                        const;


/// clamps ---------------------------------------------------------------------
	        Vector3f& clampToMinOf( const Vector3f& );
	        Vector3f& clampToMaxOf( const Vector3f& );
	        Vector3f& clampBetween( const Vector3f& min,
	                                const Vector3f& max );
	        /**
	         * 0 to almost 1, ie: [0,1).
	         */
	        Vector3f& clamp01();
	        /**
	         * 0 to almost 1, ie: [0,1).
	         */
	        void      clamp01( Vector3f& result )                          const;


/// constants ------------------------------------------------------------------
	static const Vector3f& ZERO();
	static const Vector3f& HALF();
	static const Vector3f& ONE();
	static const Vector3f& EPSILON();
	static const Vector3f& ONE_MINUS_EPSILON();
	static const Vector3f& ALMOST_ONE();
	static const Vector3f& MIN();
	static const Vector3f& MAX();
	static const Vector3f& SMALL();
	static const Vector3f& LARGE();
	static const Vector3f& X();
	static const Vector3f& Y();
	static const Vector3f& Z();


/// fields ---------------------------------------------------------------------
public:
	float x_m;
	float y_m;
	float z_m;
};








/// friends
Vector3f operator*( float, const Vector3f& );
Vector3f operator/( float, const Vector3f& );




/// streaming
std::ostream& operator<<( std::ostream&, const Vector3f& );
std::istream& operator>>( std::istream&,       Vector3f& );




/// INLINES ///


/// standard object services ---------------------------------------------------
inline
Vector3f::~Vector3f()
{
}




/// access ---------------------------------------------------------------------
inline
float Vector3f::getX() const
{
	return x_m;
}


inline
float Vector3f::getY() const
{
	return y_m;
}


inline
float Vector3f::getZ() const
{
	return z_m;
}


}//namespace




#endif//Vector3f_h
