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


#ifndef Matrix3f_h
#define Matrix3f_h


#include "Vector3f.hpp"



#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{


/**
 * A basic 3D matrix of floats, with extra translation capability.<br/><br/>
 *
 * Appropriate for general 3D transforms, not perspective projection. More
 * efficient than using a 4D matrix.
 */
class Matrix3f
{
/// standard object services ---------------------------------------------------
public:
	         Matrix3f();
	         Matrix3f( const Vector3f& row0,
	                   const Vector3f& row1,
	                   const Vector3f& row2,
	                   const Vector3f& col3 );
	         Matrix3f( const Vector3f& part0,
	                   const Vector3f& part1,
	                   const Vector3f& part2,
	                   const Vector3f& col3,
	                   bool            isRows );
	         Matrix3f( float, float, float, float,
	                   float, float, float, float,
	                   float, float, float, float );

	        ~Matrix3f();
	         Matrix3f( const Matrix3f& );
	Matrix3f& operator=( const Matrix3f& );


/// commands -------------------------------------------------------------------
	        Matrix3f& setRowsCol( const Vector3f& row0,
	                              const Vector3f& row1,
	                              const Vector3f& row2,
	                              const Vector3f& col3 );
	        Matrix3f& setColumns( const Vector3f& col0,
	                              const Vector3f& col1,
	                              const Vector3f& col2,
	                              const Vector3f& col3 );
	        Matrix3f& setElements( float, float, float, float,
	                               float, float, float, float,
	                               float, float, float, float );

	        /**
	         * this = param * this.
	         */
	        Matrix3f& multiplyEq( const Matrix3f& );
	        /**
	         * this = this * param.
	         */
	        //Matrix3f& multiplyEqPost( const Matrix3f& );
	        Matrix3f& scale( const Vector3f& );
	        bool      invert();

	        Matrix3f& setToScale    ( const Vector3f& );
	        Matrix3f& setToTranslate( const Vector3f& );
	        Matrix3f& setToRotateX  ( float radians );
	        Matrix3f& setToRotateY  ( float radians );
	        Matrix3f& setToRotateZ  ( float radians );
	        Matrix3f& setToRotateXYZ( float radiansX,
	                                  float radiansY,
	                                  float radiansZ );


/// queries --------------------------------------------------------------------
	        const Vector3f& getRow0()                                      const;
	        const Vector3f& getRow1()                                      const;
	        const Vector3f& getRow2()                                      const;
	        const Vector3f& getCol3()                                      const;
	        void            getRows( Vector3f& row0,
	                                 Vector3f& row1,
	                                 Vector3f& row2 )                      const;
	        void            getCols( Vector3f& col0,
	                                 Vector3f& col1,
	                                 Vector3f& col2 )                      const;

	        //void            getRotations( Vector3f& )                    const;

	        void            multiply( const Vector3f&,
	                                  Vector3f& )                          const;
	        Vector3f&       multiply( Vector3f& )                          const;
	        Vector3f        multiply( const Vector3f& )                    const;

	        bool            operator==( const Matrix3f& )                  const;
	        bool            operator!=( const Matrix3f& )                  const;


/// constants ------------------------------------------------------------------
	static const Matrix3f& ZERO();
	static const Matrix3f& IDENTITY();


/// implementation -------------------------------------------------------------
protected:
	static  void sinAndcos( float  angle,
	                        float& sin,
	                        float& cos );


/// fields ---------------------------------------------------------------------
private:
	Vector3f row0_m;
	Vector3f row1_m;
	Vector3f row2_m;
	Vector3f col3_m;
};


}//namespace




#endif//Matrix3f_h
