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


#include <math.h>
#include <iostream>

#include "Vector3f.hpp"


using namespace hxa7241_graphics;




/// standard object services ---------------------------------------------------
Vector3f::Vector3f()
 :	x_m(),
	y_m(),
	z_m()
{
}


Vector3f::Vector3f
(
	const float x,
	const float y,
	const float z
)
 :	x_m( x )
 ,	y_m( y )
 ,	z_m( z )
{
}


Vector3f::Vector3f
(
	const float xyz[3]
)
 :	x_m( xyz[0] )
 ,	y_m( xyz[1] )
 ,	z_m( xyz[2] )
{
}


Vector3f::Vector3f
(
	const Vector3f& v
)
 :	x_m( v.x_m )
 ,	y_m( v.y_m )
 ,	z_m( v.z_m )
{
}


Vector3f& Vector3f::operator=
(
	const Vector3f& v
)
{
	x_m = v.x_m;
	y_m = v.y_m;
	z_m = v.z_m;

	return *this;
}




/// streaming ------------------------------------------------------------------
std::ostream& hxa7241_graphics::operator<<
(
	std::ostream&   out,
	const Vector3f& obj
)
{
	return out << "(" << obj.x_m << ", " << obj.y_m << ", " << obj.z_m << ")";
}


std::istream& hxa7241_graphics::operator>>
(
	std::istream& in,
	Vector3f&     obj
)
{
	char c;
	return in >> c >> obj.x_m >> c >> obj.y_m >> c >> obj.z_m >> c;
}




/// access ---------------------------------------------------------------------
Vector3f& Vector3f::setXYZ
(
	const float x,
	const float y,
	const float z
)
{
	x_m = x;
	y_m = y;
	z_m = z;

	return *this;
}


Vector3f& Vector3f::setXYZ
(
	const float xyz[3]
)
{
	x_m = xyz[0];
	y_m = xyz[1];
	z_m = xyz[2];

	return *this;
}


void Vector3f::getXYZ
(
	float& x,
	float& y,
	float& z
) const
{
	x = x_m;
	y = y_m;
	z = z_m;
}


void Vector3f::getXYZ
(
	float xyz[3]
) const
{
	xyz[0] = x_m;
	xyz[1] = y_m;
	xyz[2] = z_m;
}




/// arithmetic -----------------------------------------------------------------
float Vector3f::sum() const
{
	return x_m + y_m + z_m;
}


float Vector3f::average() const
{
	static const float ONE_OVER_3 = 1.0f / 3.0f;

	return (x_m + y_m + z_m) * ONE_OVER_3;
}


float Vector3f::smallest() const
{
	float smallest;
	smallest = x_m      <= y_m ? x_m      : y_m;
	smallest = smallest <= z_m ? smallest : z_m;

	return smallest;
}


float Vector3f::largest() const
{
	float largest;
	largest = x_m     >= y_m ? x_m     : y_m;
	largest = largest >= z_m ? largest : z_m;

	return largest;
}


float Vector3f::length() const
{
	return float(::sqrtf( (x_m * x_m) +
	                      (y_m * y_m) +
	                      (z_m * z_m) ));
}


float Vector3f::dot
(
	const Vector3f& v
) const
{
	return (x_m * v.x_m) +
	       (y_m * v.y_m) +
	       (z_m * v.z_m);
}


float Vector3f::distance
(
	const Vector3f& v
) const
{
	const float xDif = x_m - v.x_m;
	const float yDif = y_m - v.y_m;
	const float zDif = z_m - v.z_m;

	return float(::sqrtf( (xDif * xDif) +
	                      (yDif * yDif) +
	                      (zDif * zDif) ));

//	Vector3f dif( *this - v );   // Vector3f dif( *this );   dif -= v;
//
//	return float(::sqrtf( dif.Dot( dif ) ));
}


Vector3f Vector3f::operator-() const
{
	return Vector3f( -x_m, -y_m, -z_m );
}


Vector3f& Vector3f::negate()
{
	x_m = -x_m;
	y_m = -y_m;
	z_m = -z_m;

	return *this;
}


Vector3f Vector3f::abs() const
{
	return Vector3f( *this ).absEq();
}


Vector3f& Vector3f::absEq()
{
	if( x_m < 0.0f )
	{
		x_m = -x_m;
	}
	if( y_m < 0.0f )
	{
		y_m = -y_m;
	}
	if( z_m < 0.0f )
	{
		z_m = -z_m;
	}

	return *this;
}


Vector3f Vector3f::unitize() const
{
	const float length = float(::sqrtf( (x_m * x_m) +
	                                    (y_m * y_m) +
	                                    (z_m * z_m) ));
	const float oneOverLength = length != 0.0f ? 1.0f / length : 0.0f;

	return Vector3f( x_m * oneOverLength,
					 y_m * oneOverLength,
					 z_m * oneOverLength );
}


Vector3f& Vector3f::unitizeEq()
{
	const float length = float(::sqrtf( (x_m * x_m) +
	                                    (y_m * y_m) +
	                                    (z_m * z_m) ));
	const float oneOverLength = length != 0.0f ? 1.0f / length : 0.0f;

	x_m *= oneOverLength;
	y_m *= oneOverLength;
	z_m *= oneOverLength;

	return *this;
}


Vector3f Vector3f::cross
(
	const Vector3f& v
) const
{
	return Vector3f( (y_m * v.z_m) - (z_m * v.y_m),
	                 (z_m * v.x_m) - (x_m * v.z_m),
	                 (x_m * v.y_m) - (y_m * v.x_m) );
}


Vector3f& Vector3f::crossEq
(
	const Vector3f& v
)
{
	const float x = (y_m * v.z_m) - (z_m * v.y_m);
	const float y = (z_m * v.x_m) - (x_m * v.z_m);
	const float z = (x_m * v.y_m) - (y_m * v.x_m);

	x_m = x;
	y_m = y;
	z_m = z;

	return *this;
}




/// plus minus -----------------------------------------------------------------
Vector3f Vector3f::operator+
(
	const Vector3f& v
) const
{
	return Vector3f( x_m + v.x_m,
	                 y_m + v.y_m,
	                 z_m + v.z_m );
}


Vector3f& Vector3f::operator+=
(
	const Vector3f& v
)
{
	x_m += v.x_m;
	y_m += v.y_m;
	z_m += v.z_m;

	return *this;
}


Vector3f Vector3f::operator-
(
	const Vector3f& v
) const
{
	return Vector3f( x_m - v.x_m,
	                 y_m - v.y_m,
	                 z_m - v.z_m );
}


Vector3f& Vector3f::operator-=
(
	const Vector3f& v
)
{
	x_m -= v.x_m;
	y_m -= v.y_m;
	z_m -= v.z_m;

	return *this;
}




/// mult div -------------------------------------------------------------------
Vector3f Vector3f::operator*
(
	const Vector3f& v
) const
{
	return Vector3f( x_m * v.x_m,
	                 y_m * v.y_m,
	                 z_m * v.z_m );
}


Vector3f& Vector3f::operator*=
(
	const Vector3f& v
)
{
	x_m *= v.x_m;
	y_m *= v.y_m;
	z_m *= v.z_m;

	return *this;
}


Vector3f Vector3f::operator/
(
	const Vector3f& v
) const
{
	return Vector3f( x_m / v.x_m,
	                 y_m / v.y_m,
	                 z_m / v.z_m );
}


Vector3f& Vector3f::operator/=
(
	const Vector3f& v
)
{
	x_m /= v.x_m;
	y_m /= v.y_m;
	z_m /= v.z_m;

	return *this;
}




Vector3f Vector3f::operator*
(
	const float f
) const
{
	return Vector3f( x_m * f,
	                 y_m * f,
	                 z_m * f );
}


Vector3f& Vector3f::operator*=
(
	const float f
)
{
	x_m *= f;
	y_m *= f;
	z_m *= f;

	return *this;
}


Vector3f Vector3f::operator/
(
	const float f
) const
{
	const float oneOverF = 1.0f / f;

	return Vector3f( x_m * oneOverF,
	                 y_m * oneOverF,
	                 z_m * oneOverF );
}


Vector3f& Vector3f::operator/=
(
	const float f
)
{
	const float oneOverF = 1.0f / f;

	x_m *= oneOverF;
	y_m *= oneOverF;
	z_m *= oneOverF;

	return *this;
}




/// pow ------------------------------------------------------------------------
Vector3f Vector3f::operator^
(
	const float f
) const
{
	return  Vector3f( *this ) ^= f;
}


Vector3f& Vector3f::operator^=
(
	const float f
)
{
	x_m = float(::pow( x_m, f ));
	y_m = float(::pow( y_m, f ));
	z_m = float(::pow( z_m, f ));

	return *this;
}


Vector3f Vector3f::operator^
(
	const Vector3f& v
) const
{
	return  Vector3f( *this ) ^= v;
}


Vector3f& Vector3f::operator^=
(
	const Vector3f& v
)
{
	x_m = float(::powf( x_m, v.x_m ));
	y_m = float(::powf( y_m, v.y_m ));
	z_m = float(::powf( z_m, v.z_m ));

	return *this;
}




/// friends --------------------------------------------------------------------
Vector3f hxa7241_graphics::operator*
(
	const float     f,
	const Vector3f& v
)
{
	return Vector3f( f * v.x_m,
	                 f * v.y_m,
	                 f * v.z_m );
}




Vector3f hxa7241_graphics::operator/
(
	const float     f,
	const Vector3f& v
)
{
	return Vector3f( f / v.x_m,
	                 f / v.y_m,
	                 f / v.z_m );
}




/// logical --------------------------------------------------------------------
bool Vector3f::operator==
(
	const Vector3f& v
) const
{
	return (x_m == v.x_m) &
	       (y_m == v.y_m) &
	       (z_m == v.z_m);
}


bool Vector3f::operator!=
(
	const Vector3f& v
) const
{
	return (x_m != v.x_m) |
	       (y_m != v.y_m) |
	       (z_m != v.z_m);
}


bool Vector3f::isZero() const
{
	return bool( (x_m == 0.0f) & (y_m == 0.0f) & (z_m == 0.0f) );
}


Vector3f Vector3f::sign() const
{
	return compare( Vector3f::ZERO() );
}


Vector3f Vector3f::compare
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m > v.x_m ? +1 : (x_m == v.x_m ? 0 : -1)),
	                 float(y_m > v.y_m ? +1 : (y_m == v.y_m ? 0 : -1)),
	                 float(z_m > v.z_m ? +1 : (z_m == v.z_m ? 0 : -1)) );
}


Vector3f Vector3f::equal
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m == v.x_m),
	                 float(y_m == v.y_m),
	                 float(z_m == v.z_m) );
}


Vector3f Vector3f::operator>
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m > v.x_m),
	                 float(y_m > v.y_m),
	                 float(z_m > v.z_m) );
}


Vector3f Vector3f::operator>=
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m >= v.x_m),
	                 float(y_m >= v.y_m),
	                 float(z_m >= v.z_m) );
}


Vector3f Vector3f::operator<
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m < v.x_m),
	                 float(y_m < v.y_m),
	                 float(z_m < v.z_m) );
}


Vector3f Vector3f::operator<=
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m <= v.x_m),
	                 float(y_m <= v.y_m),
	                 float(z_m <= v.z_m) );
}




/// clamps ---------------------------------------------------------------------
Vector3f& Vector3f::clampToMinOf
(
	const Vector3f& min
)
{
	if( x_m < min.x_m )
	{
		x_m = min.x_m;
	}
	if( y_m < min.y_m )
	{
		y_m = min.y_m;
	}
	if( z_m < min.z_m )
	{
		z_m = min.z_m;
	}

	return *this;
}


Vector3f& Vector3f::clampToMaxOf
(
	const Vector3f& max
)
{
	if( x_m > max.x_m )
	{
		x_m = max.x_m;
	}
	if( y_m > max.y_m )
	{
		y_m = max.y_m;
	}
	if( z_m > max.z_m )
	{
		z_m = max.z_m;
	}

	return *this;
}


Vector3f& Vector3f::clampBetween
(
	const Vector3f& min,
	const Vector3f& max
)
{
	if( x_m > max.x_m )
		x_m = max.x_m;
	else
	if( x_m < min.x_m )
		x_m = min.x_m;

	if( y_m > max.y_m )
		y_m = max.y_m;
	else
	if( y_m < min.y_m )
		y_m = min.y_m;

	if( z_m > max.z_m )
		z_m = max.z_m;
	else
	if( z_m < min.z_m )
		z_m = min.z_m;

	return *this;
}


Vector3f& Vector3f::clamp01()   // [0,1)  0 to almost 1
{
	if( x_m >= 1.0f )
		x_m  = FLOAT_ALMOST_ONE;
	else
	if( x_m < 0.0f )
		x_m = 0.0f;

	if( y_m >= 1.0f )
		y_m  = FLOAT_ALMOST_ONE;
	else
	if( y_m < 0.0f )
		y_m = 0.0f;

	if( z_m >= 1.0f )
		z_m  = FLOAT_ALMOST_ONE;
	else
	if( z_m < 0.0f )
		z_m = 0.0f;

	return *this;
}


void Vector3f::clamp01
(
	Vector3f& result
) const   // [0,1)  0 to almost 1
{
	if( x_m >= 1.0f )
		result.x_m = FLOAT_ALMOST_ONE;
	else
	if( x_m < 0.0f )
		result.x_m = 0.0f;
	else
		result.x_m = x_m;

	if( y_m >= 1.0f )
		result.y_m = FLOAT_ALMOST_ONE;
	else
	if( y_m < 0.0f )
		result.y_m = 0.0f;
	else
		result.y_m = y_m;

	if( z_m >= 1.0f )
		result.z_m = FLOAT_ALMOST_ONE;
	else
	if( z_m < 0.0f )
		result.z_m = 0.0f;
	else
		result.z_m = z_m;

//	result = *this;
//	result.clamp01();
}




/// constants ------------------------------------------------------------------
const Vector3f& Vector3f::ZERO()
{
	static const Vector3f k( 0.0f, 0.0f, 0.0f );
	return k;
}


const Vector3f& Vector3f::HALF()
{
	static const Vector3f k( 0.5f, 0.5f, 0.5f );
	return k;
}


const Vector3f& Vector3f::ONE()
{
	static const Vector3f k( 1.0f, 1.0f, 1.0f );
	return k;
}


const Vector3f& Vector3f::EPSILON()
{
	static const Vector3f k( FLOAT_EPSILON, FLOAT_EPSILON, FLOAT_EPSILON );
	return k;
}


const Vector3f& Vector3f::ONE_MINUS_EPSILON()
{
	static const Vector3f k( FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE );
	return k;
}


const Vector3f& Vector3f::ALMOST_ONE()
{
	static const Vector3f k( FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE );
	return k;
}


const Vector3f& Vector3f::MIN()
{
	static const Vector3f k( FLOAT_MIN_NEG, FLOAT_MIN_NEG, FLOAT_MIN_NEG );
	return k;
}


const Vector3f& Vector3f::MAX()
{
	static const Vector3f k( FLOAT_MAX, FLOAT_MAX, FLOAT_MAX );
	return k;
}


const Vector3f& Vector3f::SMALL()
{
	static const Vector3f k( FLOAT_SMALL, FLOAT_SMALL, FLOAT_SMALL );
	return k;
}


const Vector3f& Vector3f::LARGE()
{
	static const Vector3f k( FLOAT_LARGE, FLOAT_LARGE, FLOAT_LARGE );
	return k;
}


const Vector3f& Vector3f::X()
{
	static const Vector3f k( 1.0f, 0.0f, 0.0f );
	return k;
}


const Vector3f& Vector3f::Y()
{
	static const Vector3f k( 0.0f, 1.0f, 0.0f );
	return k;
}


const Vector3f& Vector3f::Z()
{
	static const Vector3f k( 0.0f, 0.0f, 1.0f );
	return k;
}
