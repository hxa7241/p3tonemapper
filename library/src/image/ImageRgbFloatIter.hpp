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


#ifndef ImageRgbFloatIter_h
#define ImageRgbFloatIter_h


#include "Vector3f.hpp"




#include "p3tonemapper_image.hpp"
namespace p3tonemapper_image
{
	using hxa7241_graphics::Vector3f;


class ImageRgbFloatIterConst
{
/// standard object services ---------------------------------------------------
public:
	         ImageRgbFloatIterConst( const float* );

	        ~ImageRgbFloatIterConst();
	         ImageRgbFloatIterConst( const ImageRgbFloatIterConst& );
	ImageRgbFloatIterConst& operator=( const ImageRgbFloatIterConst& );


/// commands -------------------------------------------------------------------
	        ImageRgbFloatIterConst& operator++();
	        ImageRgbFloatIterConst& operator++( int );
	        ImageRgbFloatIterConst& operator--();
	        ImageRgbFloatIterConst& operator--( int );

	        ImageRgbFloatIterConst& operator+=( dword );
	        ImageRgbFloatIterConst& operator-=( dword );


/// queries --------------------------------------------------------------------
	        ImageRgbFloatIterConst  operator+( dword )                     const;
	        ImageRgbFloatIterConst  operator-( dword )                     const;

	        bool     operator< ( const ImageRgbFloatIterConst& )           const;
	        bool     operator> ( const ImageRgbFloatIterConst& )           const;
	        bool     operator<=( const ImageRgbFloatIterConst& )           const;
	        bool     operator>=( const ImageRgbFloatIterConst& )           const;
	        bool     operator==( const ImageRgbFloatIterConst& )           const;
	        bool     operator!=( const ImageRgbFloatIterConst& )           const;

	        void     get( Vector3f& )                                      const;
	        Vector3f get()                                                 const;


/// fields ---------------------------------------------------------------------
private:
	const float* pFloat_m;
};




/// INLINES ///

/// standard object services ---------------------------------------------------
inline
ImageRgbFloatIterConst::ImageRgbFloatIterConst
(
	const float* pFloat
)
 :	pFloat_m( pFloat )
{
}


inline
ImageRgbFloatIterConst::~ImageRgbFloatIterConst()
{
}


inline
ImageRgbFloatIterConst::ImageRgbFloatIterConst
(
	const ImageRgbFloatIterConst& other
)
 :	pFloat_m( other.pFloat_m )
{
}


inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator=
(
	const ImageRgbFloatIterConst& other
)
{
	pFloat_m = other.pFloat_m;

	return *this;
}




/// commands -------------------------------------------------------------------
inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator++()
{
	pFloat_m += 3;
	return *this;
}


inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator++( int )
{
	pFloat_m += 3;
	return *this;
}


inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator--()
{
	pFloat_m -= 3;
	return *this;
}


inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator--( int )
{
	pFloat_m -= 3;
	return *this;
}


inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator+=
(
	const dword inc
)
{
	pFloat_m += inc * 3;
	return *this;
}


inline
ImageRgbFloatIterConst& ImageRgbFloatIterConst::operator-=
(
	const dword dec
)
{
	pFloat_m -= dec * 3;
	return *this;
}




/// queries --------------------------------------------------------------------
inline
ImageRgbFloatIterConst ImageRgbFloatIterConst::operator+
(
	const dword inc
) const
{
	return ImageRgbFloatIterConst( pFloat_m + (inc * 3) );
}


inline
ImageRgbFloatIterConst ImageRgbFloatIterConst::operator-
(
	const dword dec
) const
{
	return ImageRgbFloatIterConst( pFloat_m - (dec * 3) );
}


inline
bool ImageRgbFloatIterConst::operator<
(
	const ImageRgbFloatIterConst& other
) const
{
	return pFloat_m < other.pFloat_m;
}


inline
bool ImageRgbFloatIterConst::operator>
(
	const ImageRgbFloatIterConst& other
) const
{
	return pFloat_m > other.pFloat_m;
}


inline
bool ImageRgbFloatIterConst::operator<=
(
	const ImageRgbFloatIterConst& other
) const
{
	return pFloat_m <= other.pFloat_m;
}


inline
bool ImageRgbFloatIterConst::operator>=
(
	const ImageRgbFloatIterConst& other
) const
{
	return pFloat_m >= other.pFloat_m;
}


inline
bool ImageRgbFloatIterConst::operator==
(
	const ImageRgbFloatIterConst& other
) const
{
	return pFloat_m == other.pFloat_m;
}


inline
bool ImageRgbFloatIterConst::operator!=
(
	const ImageRgbFloatIterConst& other
) const
{
	return pFloat_m != other.pFloat_m;
}


inline
void ImageRgbFloatIterConst::get
(
	Vector3f& value
) const
{
	value.setXYZ( pFloat_m );
}


inline
Vector3f ImageRgbFloatIterConst::get() const
{
	return Vector3f( pFloat_m );
}








class ImageRgbFloatIter
{
/// standard object services ---------------------------------------------------
public:
	         ImageRgbFloatIter( float* );

	        ~ImageRgbFloatIter();
	         ImageRgbFloatIter( const ImageRgbFloatIter& );
	ImageRgbFloatIter& operator=( const ImageRgbFloatIter& );


/// commands -------------------------------------------------------------------
	        ImageRgbFloatIter& operator++();
	        ImageRgbFloatIter& operator++( int );
	        ImageRgbFloatIter& operator--();
	        ImageRgbFloatIter& operator--( int );

	        ImageRgbFloatIter& operator+=( dword );
	        ImageRgbFloatIter& operator-=( dword );

	        void     set( const Vector3f& );


/// queries --------------------------------------------------------------------
	        ImageRgbFloatIter  operator+( dword )                          const;
	        ImageRgbFloatIter  operator-( dword )                          const;

	        bool     operator< ( const ImageRgbFloatIter& )                const;
	        bool     operator> ( const ImageRgbFloatIter& )                const;
	        bool     operator<=( const ImageRgbFloatIter& )                const;
	        bool     operator>=( const ImageRgbFloatIter& )                const;
	        bool     operator==( const ImageRgbFloatIter& )                const;
	        bool     operator!=( const ImageRgbFloatIter& )                const;

	        void     get( Vector3f& )                                      const;
	        Vector3f get()                                                 const;


/// fields ---------------------------------------------------------------------
private:
	float* pFloat_m;
};




/// INLINES ///

/// standard object services ---------------------------------------------------
inline
ImageRgbFloatIter::ImageRgbFloatIter
(
	float* pFloat
)
 :	pFloat_m( pFloat )
{
}


inline
ImageRgbFloatIter::~ImageRgbFloatIter()
{
}


inline
ImageRgbFloatIter::ImageRgbFloatIter
(
	const ImageRgbFloatIter& other
)
 :	pFloat_m( other.pFloat_m )
{
}


inline
ImageRgbFloatIter& ImageRgbFloatIter::operator=
(
	const ImageRgbFloatIter& other
)
{
	pFloat_m = other.pFloat_m;

	return *this;
}




/// commands -------------------------------------------------------------------
inline
ImageRgbFloatIter& ImageRgbFloatIter::operator++()
{
	pFloat_m += 3;
	return *this;
}


inline
ImageRgbFloatIter& ImageRgbFloatIter::operator++( int )
{
	pFloat_m += 3;
	return *this;
}


inline
ImageRgbFloatIter& ImageRgbFloatIter::operator--()
{
	pFloat_m -= 3;
	return *this;
}


inline
ImageRgbFloatIter& ImageRgbFloatIter::operator--( int )
{
	pFloat_m -= 3;
	return *this;
}


inline
ImageRgbFloatIter& ImageRgbFloatIter::operator+=
(
	const dword inc
)
{
	pFloat_m += inc * 3;
	return *this;
}


inline
ImageRgbFloatIter& ImageRgbFloatIter::operator-=
(
	const dword dec
)
{
	pFloat_m -= dec * 3;
	return *this;
}


inline
void ImageRgbFloatIter::set
(
	const Vector3f& value
)
{
	Vector3f valueClamped( value );
	valueClamped.clampBetween( Vector3f::ZERO(), Vector3f::LARGE() );

	valueClamped.getXYZ( pFloat_m );
}




/// queries --------------------------------------------------------------------
inline
ImageRgbFloatIter ImageRgbFloatIter::operator+
(
	const dword inc
) const
{
	return ImageRgbFloatIter( pFloat_m + (inc * 3) );
}


inline
ImageRgbFloatIter ImageRgbFloatIter::operator-
(
	const dword dec
) const
{
	return ImageRgbFloatIter( pFloat_m - (dec * 3) );
}


inline
bool ImageRgbFloatIter::operator<
(
	const ImageRgbFloatIter& other
) const
{
	return pFloat_m < other.pFloat_m;
}


inline
bool ImageRgbFloatIter::operator>
(
	const ImageRgbFloatIter& other
) const
{
	return pFloat_m > other.pFloat_m;
}


inline
bool ImageRgbFloatIter::operator<=
(
	const ImageRgbFloatIter& other
) const
{
	return pFloat_m <= other.pFloat_m;
}


inline
bool ImageRgbFloatIter::operator>=
(
	const ImageRgbFloatIter& other
) const
{
	return pFloat_m >= other.pFloat_m;
}


inline
bool ImageRgbFloatIter::operator==
(
	const ImageRgbFloatIter& other
) const
{
	return pFloat_m == other.pFloat_m;
}


inline
bool ImageRgbFloatIter::operator!=
(
	const ImageRgbFloatIter& other
) const
{
	return pFloat_m != other.pFloat_m;
}


inline
void ImageRgbFloatIter::get
(
	Vector3f& value
) const
{
	value.setXYZ( pFloat_m );
}


inline
Vector3f ImageRgbFloatIter::get() const
{
	return Vector3f( pFloat_m );
}




}//namespace




#endif//ImageRgbFloatIter_h
