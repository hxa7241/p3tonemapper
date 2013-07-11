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

#include "Matrix3f.hpp"


using namespace hxa7241_graphics;




/// standard object services ---------------------------------------------------
Matrix3f::Matrix3f()
 :	row0_m()
 ,	row1_m()
 ,	row2_m()
 ,	col3_m()
{
}


Matrix3f::Matrix3f
(
	const Vector3f& row0,
	const Vector3f& row1,
	const Vector3f& row2,
	const Vector3f& col3
)
 :	row0_m( row0 )
 ,	row1_m( row1 )
 ,	row2_m( row2 )
 ,	col3_m( col3 )
{
}


Matrix3f::Matrix3f
(
	const Vector3f& part0,
	const Vector3f& part1,
	const Vector3f& part2,
	const Vector3f& col3,
	const bool      isRows
)
 :	row0_m()
 ,	row1_m()
 ,	row2_m()
 ,	col3_m()
{
	if( isRows )
	{
		Matrix3f::setRowsCol( part0, part1, part2, col3 );
	}
	else
	{
		Matrix3f::setColumns( part0, part1, part2, col3 );
	}
}


Matrix3f::Matrix3f
(
	const float m00,  const float m10,  const float m20,  const float m30,
	const float m01,  const float m11,  const float m21,  const float m31,
	const float m02,  const float m12,  const float m22,  const float m32
)
 :	row0_m( m00, m10, m20 )
 ,	row1_m( m01, m11, m21 )
 ,	row2_m( m02, m12, m22 )
 ,	col3_m( m30, m31, m32 )
{
}


Matrix3f::~Matrix3f()
{
}


Matrix3f::Matrix3f
(
	const Matrix3f& other
)
 :	row0_m( other.row0_m )
 ,	row1_m( other.row1_m )
 ,	row2_m( other.row2_m )
 ,	col3_m( other.col3_m )
{
}


Matrix3f& Matrix3f::operator=
(
	const Matrix3f& other
)
{
	if( &other != this )
	{
		row0_m = other.row0_m;
		row1_m = other.row1_m;
		row2_m = other.row2_m;
		col3_m = other.col3_m;
	}

	return  *this;
}




/// commands -------------------------------------------------------------------
Matrix3f& Matrix3f::setRowsCol
(
	const Vector3f& row0,
	const Vector3f& row1,
	const Vector3f& row2,
	const Vector3f& col3
)
{
	row0_m = row0;
	row1_m = row1;
	row2_m = row2;
	col3_m = col3;

	return *this;
}


Matrix3f& Matrix3f::setColumns
(
	const Vector3f& col0,
	const Vector3f& col1,
	const Vector3f& col2,
	const Vector3f& col3
)
{
	row0_m.setXYZ( col0.getX(), col1.getX(), col2.getX() );
	row1_m.setXYZ( col0.getY(), col1.getY(), col2.getY() );
	row2_m.setXYZ( col0.getZ(), col1.getZ(), col2.getZ() );
	col3_m = col3;

	return *this;
}


Matrix3f& Matrix3f::setElements
(
	const float m00,  const float m10,  const float m20,  const float m30,
	const float m01,  const float m11,  const float m21,  const float m31,
	const float m02,  const float m12,  const float m22,  const float m32
)
{
	row0_m.setXYZ( m00, m10, m20 );
	row1_m.setXYZ( m01, m11, m21 );
	row2_m.setXYZ( m02, m12, m22 );
	col3_m.setXYZ( m30, m31, m32 );

	return *this;
}


Matrix3f& Matrix3f::multiplyEq
(
	const Matrix3f& m2
)
{
	// this  =  m2 . this

	Vector3f col0;
	Vector3f col1;
	Vector3f col2;

	getCols( col0,  col1,  col2 );

	row0_m.setXYZ( m2.row0_m.dot( col0 ),
	               m2.row0_m.dot( col1 ),
	               m2.row0_m.dot( col2 ) ),
	row1_m.setXYZ( m2.row1_m.dot( col0 ),
	               m2.row1_m.dot( col1 ),
	               m2.row1_m.dot( col2 ) ),
	row2_m.setXYZ( m2.row2_m.dot( col0 ),
	               m2.row2_m.dot( col1 ),
	               m2.row2_m.dot( col2 ) );

	col3_m.setXYZ( col3_m.dot( m2.row0_m ),
	               col3_m.dot( m2.row1_m ),
	               col3_m.dot( m2.row2_m ) );

	col3_m += m2.col3_m;


	return *this;

//	// reverse order
//	Vector3f  NewCol3( m2.Col3_m.Dot( Row0_m ),
//					 m2.Col3_m.Dot( Row1_m ),
//					 m2.Col3_m.Dot( Row2_m ) );
//
//	Col3_m  +=  NewCol3;
//
//
//	Vector3f  m2Col0( m2.Row0_m.GetX(),  m2.Row1_m.GetX(),  m2.Row2_m.GetX() ),
//			m2Col1( m2.Row0_m.GetY(),  m2.Row1_m.GetY(),  m2.Row2_m.GetY() ),
//			m2Col2( m2.Row0_m.GetZ(),  m2.Row1_m.GetZ(),  m2.Row2_m.GetZ() );
//
//	Row0_m.SetXYZ( Row0_m.Dot( m2Col0 ),  Row0_m.Dot( m2Col1 ),  Row0_m.Dot( m2Col2 ) );
//	Row1_m.SetXYZ( Row1_m.Dot( m2Col0 ),  Row1_m.Dot( m2Col1 ),  Row1_m.Dot( m2Col2 ) );
//	Row2_m.SetXYZ( Row2_m.Dot( m2Col0 ),  Row2_m.Dot( m2Col1 ),  Row2_m.Dot( m2Col2 ) );
//
//
//	return  *this;
}


Matrix3f& Matrix3f::scale
(
	const Vector3f& v
)
{
	row0_m *= v;
	row1_m *= v;
	row2_m *= v;

	return *this;
}


bool Matrix3f::invert()
{

	float m[3][3];
	row0_m.getXYZ( m[0][0], m[1][0], m[2][0] );
	row1_m.getXYZ( m[0][1], m[1][1], m[2][1] );
	row2_m.getXYZ( m[0][2], m[1][2], m[2][2] );


	const float determinant =
		( m[0][0] * m[1][1] * m[2][2] ) - ( m[0][0] * m[2][1] * m[1][2] )  +
		( m[1][0] * m[2][1] * m[0][2] ) - ( m[1][0] * m[0][1] * m[2][2] )  +
		( m[2][0] * m[0][1] * m[1][2] ) - ( m[2][0] * m[1][1] * m[0][2] );
	const float oneOverDet =
		(determinant == 0.0f) ? FLOAT_LARGE : (1.0f / determinant);


	row0_m.setXYZ(  ( m[1][1] * m[2][2] - m[1][2] * m[2][1] ) * oneOverDet,
	               -( m[1][0] * m[2][2] - m[1][2] * m[2][0] ) * oneOverDet,
	                ( m[1][0] * m[2][1] - m[1][1] * m[2][0] ) * oneOverDet );

	row1_m.setXYZ( -( m[0][1] * m[2][2] - m[0][2] * m[2][1] ) * oneOverDet,
	                ( m[0][0] * m[2][2] - m[0][2] * m[2][0] ) * oneOverDet,
	               -( m[0][0] * m[2][1] - m[0][1] * m[2][0] ) * oneOverDet );

	row2_m.setXYZ(  ( m[0][1] * m[1][2] - m[0][2] * m[1][1] ) * oneOverDet,
	               -( m[0][0] * m[1][2] - m[0][2] * m[1][0] ) * oneOverDet,
	                ( m[0][0] * m[1][1] - m[0][1] * m[1][0] ) * oneOverDet );


	const Vector3f negCol3( -col3_m );

	col3_m.setXYZ( negCol3.dot( row0_m ),
	               negCol3.dot( row1_m ),
	               negCol3.dot( row2_m ) );


	return (determinant > FLOAT_SMALL) | (determinant < -FLOAT_SMALL);


//	// array[][] versn
//	const float det =
//		( m[0][0] * m[1][1] * m[2][2] ) - ( m[0][0] * m[2][1] * m[1][2] ) +
//		( m[1][0] * m[2][1] * m[0][2] ) - ( m[1][0] * m[0][1] * m[2][2] ) +
//		( m[2][0] * m[0][1] * m[1][2] ) - ( m[2][0] * m[1][1] * m[0][2] );
//	const float oneOverDet = (det == 0.0f) ? FLOAT_LARGE : (1.0f / det);
//
//	Matrix3f result;
//
//	result.m[0][0] =  ( m[1][1] * m[2][2] - m[1][2] * m[2][1] ) * oneOverDet;
//	result.m[1][0] = -( m[1][0] * m[2][2] - m[1][2] * m[2][0] ) * oneOverDet;
//	result.m[2][0] =  ( m[1][0] * m[2][1] - m[1][1] * m[2][0] ) * oneOverDet;
//
//	result.m[0][1] = -( m[0][1] * m[2][2] - m[0][2] * m[2][1] ) * oneOverDet;
//	result.m[1][1] =  ( m[0][0] * m[2][2] - m[0][2] * m[2][0] ) * oneOverDet;
//	result.m[2][1] = -( m[0][0] * m[2][1] - m[0][1] * m[2][0] ) * oneOverDet;
//
//	result.m[0][2] =  ( m[0][1] * m[1][2] - m[0][2] * m[1][1] ) * oneOverDet;
//	result.m[1][2] = -( m[0][0] * m[1][2] - m[0][2] * m[1][0] ) * oneOverDet;
//	result.m[2][2] =  ( m[0][0] * m[1][1] - m[0][1] * m[1][0] ) * oneOverDet;
//
//	*this = result;
//
//	return det != 0.0f;
}


Matrix3f& Matrix3f::setToScale
(
	const Vector3f& xyz
)
{
	row0_m.setXYZ( xyz.getX(), 0.0f, 0.0f );
	row1_m.setXYZ( 0.0f, xyz.getY(), 0.0f );
	row2_m.setXYZ( 0.0f, 0.0f, xyz.getZ() );
	col3_m = Vector3f::ZERO();

	return *this;
}


Matrix3f& Matrix3f::setToTranslate
(
	const Vector3f& xyz
)
{
	*this  = IDENTITY();
	col3_m = xyz;

	return *this;
}


Matrix3f& Matrix3f::setToRotateX
(
	const float radians
)
{
	float sin;
	float cos;
	sinAndcos( radians, sin, cos );

	row0_m.setXYZ( 1.0f, 0.0f, 0.0f );
	row1_m.setXYZ( 0.0f,  cos,  sin );
	row2_m.setXYZ( 0.0f, -sin,  cos );
	col3_m = Vector3f::ZERO();

	return *this;
}


Matrix3f& Matrix3f::setToRotateY
(
	const float radians
)
{
	float sin;
	float cos;
	sinAndcos( radians, sin, cos );

	row0_m.setXYZ(  cos, 0.0f, -sin );
	row1_m.setXYZ( 0.0f, 1.0f, 0.0f );
	row2_m.setXYZ(  sin, 0.0f,  cos );
	col3_m = Vector3f::ZERO();

	return *this;
}


Matrix3f& Matrix3f::setToRotateZ
(
	const float radians
)
{
	float sin;
	float cos;
	sinAndcos( radians, sin, cos );

	row0_m.setXYZ(  cos,  sin, 0.0f );
	row1_m.setXYZ( -sin,  cos, 0.0f );
	row2_m.setXYZ( 0.0f, 0.0f, 1.0f );
	col3_m = Vector3f::ZERO();

	return *this;
}


Matrix3f& Matrix3f::setToRotateXYZ
(
	const float radiansX,
	const float radiansY,
	const float radiansZ
)
{
	float sinX;
	float cosX;
	sinAndcos( radiansX, sinX, cosX );
	float sinY;
	float cosY;
	sinAndcos( radiansY, sinY, cosY );
	float sinZ;
	float cosZ;
	sinAndcos( radiansZ, sinZ, cosZ );

	const float cosXsinY = cosX * sinY;
	const float sinXsinY = sinX * sinY;

	row0_m.setXYZ(   cosY * cosZ,
	               ( sinXsinY * cosZ) + (cosX * sinZ),
	               (-cosXsinY * cosZ) + (sinX * sinZ) );
	row1_m.setXYZ(  -cosY * sinZ,
	               (-sinXsinY * sinZ) + (cosX * cosZ),
	               ( cosXsinY * sinZ) + (sinX * cosZ) );
	row2_m.setXYZ(  sinY,
	               -sinX * cosY,
	                cosX * cosY );
	col3_m = Vector3f::ZERO();

	return *this;
}




/// queries --------------------------------------------------------------------
const Vector3f& Matrix3f::getRow0() const
{
	return row0_m;
}


const Vector3f& Matrix3f::getRow1() const
{
	return row1_m;
}


const Vector3f& Matrix3f::getRow2() const
{
	return row2_m;
}


const Vector3f& Matrix3f::getCol3() const
{
	return col3_m;
}


void Matrix3f::getRows
(
	Vector3f& row0,
	Vector3f& row1,
	Vector3f& row2
) const
{
	row0 = row0_m;
	row1 = row1_m;
	row2 = row2_m;
}


void Matrix3f::getCols
(
	Vector3f& col0,
	Vector3f& col1,
	Vector3f& col2
) const
{
	col0.setXYZ( row0_m.getX(), row1_m.getX(), row2_m.getX() );
	col1.setXYZ( row0_m.getY(), row1_m.getY(), row2_m.getY() );
	col2.setXYZ( row0_m.getZ(), row1_m.getZ(), row2_m.getZ() );
}


/*void Matrix3f::getRotations
(
	Vector3f& rotations
) const
{
	// algorithm from 'the matrix and quaternion faq'
	// doesnt work properly

	float angleX;// = 0.0f;
	float angleY;// = 0.0f;
	float angleZ;// = 0.0f;

	const float d = float(::asinf( row2_m.getX() ));
	const float c = float(::cosf( d ));

	angleY = d;

	if( float(::fabsf(c)) > 0.005f )
	{
		const float oneOverC = 1.0f / c;

		const float trX1 =  row2_m.getZ() * oneOverC;
		const float trY1 = -row2_m.getY() * oneOverC;

		angleX = float(::atan2f( trY1, trX1 ));

		const float trX2 =  row0_m.getX() * oneOverC;
		const float trY2 = -row1_m.getX() * oneOverC;

		angleZ = float(::atan2f( trY2, trX2 ));
	}
	else
	{
		angleX = 0.0f;

		const float trX =  row1_m.getY();
		const float trY = -row0_m.getY();

		angleZ = float(::atan2f( trY, trX ));
	}

	rotations.setXYZ( angleX, angleY, angleZ );
}*/


void Matrix3f::multiply
(
	const Vector3f& v,
	Vector3f&       result
) const
{
	result.setXYZ( v.dot( row0_m ),
	               v.dot( row1_m ),
	               v.dot( row2_m ) );

	result += col3_m;
}


Vector3f& Matrix3f::multiply
(
	Vector3f& v
) const
{
	v.setXYZ( v.dot( row0_m ),
	          v.dot( row1_m ),
	          v.dot( row2_m ) );
	v += col3_m;

	return v;
}


Vector3f Matrix3f::multiply
(
	const Vector3f& v
) const
{
	Vector3f result( v.dot( row0_m ),
	                 v.dot( row1_m ),
	                 v.dot( row2_m ) );
	result += col3_m;

	return result;
}


//Vector3f Matrix3f::operator*
//(
//	const Vector3f& v
//) const
//{
//	Vector3f v2;
//
//	multiply( v, v2 );
//
//	return v2;
//}


bool Matrix3f::operator==
(
	const Matrix3f& m2
) const
{
	return ( row0_m == m2.row0_m ) &&
	       ( row1_m == m2.row1_m ) &&
	       ( row2_m == m2.row2_m ) &&
	       ( col3_m == m2.col3_m );
}


bool Matrix3f::operator!=
(
	const Matrix3f& m2
) const
{
	return !operator==( m2 );
}




/// constants ------------------------------------------------------------------
const Matrix3f& Matrix3f::ZERO()
{
	static const Matrix3f k( 0.0f, 0.0f, 0.0f, 0.0f,
	                         0.0f, 0.0f, 0.0f, 0.0f,
	                         0.0f, 0.0f, 0.0f, 0.0f );
	return k;
}


const Matrix3f& Matrix3f::IDENTITY()
{
	static const Matrix3f k( 1.0f, 0.0f, 0.0f, 0.0f,
	                         0.0f, 1.0f, 0.0f, 0.0f,
	                         0.0f, 0.0f, 1.0f, 0.0f );
	return k;
}




/// implementation -------------------------------------------------------------
void Matrix3f::sinAndcos
(
	const float angle,
	float&      sin,
	float&      cos
)
{
#ifdef BORLANDCPP551
	// use the intel single instruction
	double sine;
	double cosine;
	::_fm_sincos( angle, &sine, &cosine );
	sin = float(sine);
	cos = float(cosine);
#else
	// use the std lib
	sin = float(::sinf( angle ));
	cos = float(::cosf( angle ));
#endif
}
