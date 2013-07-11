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


#ifndef Array_h
#define Array_h




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/**
 * A simpler, compacter alternative to std::vector.<br/><br/>
 *
 * Length is explicit - there is no hidden reserve.<br/>
 * Non-owning reference capability.<br/><br/>
 *
 * isAdopt false means make a non-owning reference to supplied storage: Array
 * will not delete the pointer. But subsequent use of copy-assignor, setLength,
 * append, or remove will revert Array back to owning new storage.<br/><br/>
 *
 * isAdopt true means take ownership of a pointer to storage: Array will delete
 * the pointer when needed.
 *
 * Copy constructor and copy assignor always make new owned storage (rather
 * than copy ref-status from a non-owning ref).<br/><br/>
 *
 * @implementation
 * free-store ethics (ethics is what one ought to do)<br/>
 * basic rules (ensuring correctness):<br/>
 * * in constructors, pointer is initialized to zero<br/>
 * * in destructor, pointer is deleted<br/>
 * * when pointer is changed:<br/>
 *   * first delete it<br/>
 *   * value changed to must be: zero, or from new<br/>
 * refinement rules:<br/>
 * * if storage is not owned, don't delete pointer<br/>
 *
 * @invariants
 * pMemory_m is 0 or a valid address
 * length_m low 15 bits is >= 0 and <= getMaxLength() (DWORD_MAX)
 * length_m highest bit is logical not of isOwning()
 */
template<class TYPE>
class Array
{
/// standard object services ---------------------------------------------------
public:
	               Array();
	explicit       Array( dword length );                               // throws
	               Array( TYPE* pMemory,
	                      dword length,
	                      bool  isAdopt );                              // throws

	virtual       ~Array();
	               Array( const Array& );                               // throws
	        Array& operator=( const Array& );                           // throws


/// commands -------------------------------------------------------------------
	virtual void   setLength( dword length );                           // throws
	virtual void   setMemory( TYPE* pMemory,
	                          dword length,
	                          bool  isAdopt );

	virtual void   swap( Array& );
	virtual void   append( const TYPE& );                               // throws
	virtual void   remove( int index );                                 // throws

	virtual void   zeroMemory();

	virtual TYPE*  getMemory();
	virtual TYPE&  operator[]( int index );
	virtual TYPE&  get( int index );


/// queries --------------------------------------------------------------------
	virtual dword  getLength()                                             const;
	virtual bool   isEmpty()                                               const;
	virtual bool   isOwning()                                              const;
	static  dword  getMaxLength();

	virtual const TYPE* getMemory()                                        const;
	virtual const TYPE& operator[]( int index )                            const;
	virtual const TYPE& get( int index )                                   const;


/// implementation -------------------------------------------------------------
protected:
	virtual void   assign( const Array<TYPE>& );

	virtual void   acquireMemory( dword length,
	                              bool  isCopied );

	static  void   copyObjects( TYPE*       lValStart,
	                            const TYPE* rValStart,
	                            dword       length );


/// fields ---------------------------------------------------------------------
private:
	TYPE* pMemory_m;
	dword length_m;

	static const char ALLOCATION_EXCEPTION_MESSAGE[];
};








/// statics --------------------------------------------------------------------
template<class TYPE>
const char Array<TYPE>::ALLOCATION_EXCEPTION_MESSAGE[] =
	"Array<>::acquireMemory() - memory allocation failed";




/// standard object services ---------------------------------------------------
template<class TYPE>
Array<TYPE>::Array()
 :	pMemory_m( 0 )
 ,	length_m ( 0 )
{
}


template<class TYPE>
Array<TYPE>::Array
(
	const dword length
)
 :	pMemory_m( 0 )
 ,	length_m ( 0 )
{
	Array<TYPE>::setLength( length );
}


template<class TYPE>
Array<TYPE>::Array
(
	TYPE*const  pMemory,
	const dword length,
	const bool  isAdopt
)
 :	pMemory_m( 0 )
 ,	length_m ( 0 )
{
	Array<TYPE>::setMemory( pMemory, length, isAdopt );
}


template<class TYPE>
Array<TYPE>::~Array()
{
	if( isOwning() )
	{
		delete[] pMemory_m;
	}
}


template<class TYPE>
Array<TYPE>::Array
(
	const Array<TYPE>& other
)
 :	pMemory_m( 0 )
 ,	length_m ( 0 )
{
	Array<TYPE>::assign( other );
}


template<class TYPE>
Array<TYPE>& Array<TYPE>::operator=
(
	const Array<TYPE>& other
)
{
	assign( other );

	return *this;
}




/// commands -------------------------------------------------------------------
template<class TYPE>
void Array<TYPE>::setLength
(
	const dword length
)
{
	acquireMemory( length, false );
}


template<class TYPE>
void Array<TYPE>::setMemory
(
	TYPE*const  pMemory,
	dword       length,
	const bool  isAdopt
)
{
	if( pMemory != pMemory_m )
	{
		if( isOwning() )
		{
			delete[] pMemory_m;
		}
		pMemory_m = pMemory;
	}

	length_m  = (length >= 0) ? length : 0;
	length_m |= (dword(!isAdopt) << 31);
}


template<class TYPE>
void Array<TYPE>::swap
(
	Array<TYPE>& other
)
{
	TYPE*const tmpM  = pMemory_m;
	pMemory_m        = other.pMemory_m;
	other.pMemory_m  = tmpM;

	const dword tmpL = length_m;
	length_m         = other.length_m;
	other.length_m   = tmpL;
}


template<class TYPE>
void Array<TYPE>::append
(
	const TYPE& element
)
{
	// expand storage, duplicating elements
	acquireMemory( getLength() + 1, true );

	// write new element into last position
	*(pMemory_m + getLength() - 1) = element;


//	// make larger storage
//	Array<TYPE> newArray( getLength() + 1 );
//
//	// duplicate elements, and append new element
//	copyObjects( newArray.pMemory_m, pMemory_m, getLength() );
//	*(newArray.pMemory_m + getLength()) = element;
//
//	// swap new storage with this
//	swap( newArray );
}


template<class TYPE>
void Array<TYPE>::remove
(
	const int index
)
{
	// check index is within range
	if( (index >= 0) & (index < getLength())  )
	{
		// make smaller storage
		Array<TYPE> newArray( getLength() - 1 );

		// copy elements, skipping element at index
		{
			      TYPE* pDestination = newArray.pMemory_m;
			      TYPE* pEnd         = pDestination + newArray.getLength();
			const TYPE* pSource      = pMemory_m;
			const TYPE* pIndex       = pSource + index;
			while( pDestination < pEnd )
			{
				pSource += dword(pSource == pIndex);
				*(pDestination++) = *(pSource++);
			}
		}

		// swap new storage with this
		swap( newArray );
	}
}


template<class TYPE>
void Array<TYPE>::zeroMemory()
{
	for( dword i = getLength();  i-- > 0; )
	{
		pMemory_m[ i ] = TYPE();
	}
}


template<class TYPE>
inline
TYPE* Array<TYPE>::getMemory()
{
	return pMemory_m;
}


template<class TYPE>
inline
TYPE& Array<TYPE>::operator[]
(
	const int index
)
{
	return pMemory_m[ index ];
}


template<class TYPE>
TYPE& Array<TYPE>::get
(
	const int index
)
{
	return pMemory_m[ (index >= 0 ? index : -index) % getLength() ];
}




/// queries --------------------------------------------------------------------
template<class TYPE>
inline
dword Array<TYPE>::getLength() const
{
	return length_m & 0x7FFFFFFF;
}


template<class TYPE>
inline
bool Array<TYPE>::isEmpty() const
{
	return getLength() == 0;
}


template<class TYPE>
inline
bool Array<TYPE>::isOwning() const
{
	return (length_m & 0x80000000) == 0;
}


template<class TYPE>
inline
dword Array<TYPE>::getMaxLength()
{
	return DWORD_MAX;
}


template<class TYPE>
inline
const TYPE* Array<TYPE>::getMemory() const
{
	return pMemory_m;
}


template<class TYPE>
inline
const TYPE& Array<TYPE>::operator[]
(
	const int index
) const
{
	return pMemory_m[ index ];
}


template<class TYPE>
const TYPE& Array<TYPE>::get
(
	const int index
) const
{
	return pMemory_m[ (index >= 0 ? index : -index) % getLength() ];
}




/// implementation -------------------------------------------------------------
template<class TYPE>
void Array<TYPE>::assign
(
	const Array<TYPE>& other
)
{
	if( &other != this )
	{
		acquireMemory( other.getLength(), false );

		copyObjects( getMemory(), other.getMemory(), other.getLength() );
	}
}


template<class TYPE>
void Array<TYPE>::acquireMemory
(
	dword      newLength,
	const bool isCopied
)
{
	// clamp to 0 min
	newLength = (newLength >= 0) ? newLength : 0;

	// only allocate if different length
	if( newLength != getLength() )
	{
		// allocate new storage
		TYPE* pNewMemory = new TYPE[ newLength ];
		if( pNewMemory == 0 )
		{
			throw ALLOCATION_EXCEPTION_MESSAGE;
		}

		// copy elements to new storage
		if( isCopied )
		{
			copyObjects( pNewMemory, pMemory_m,
				(getLength() <= newLength ? getLength() : newLength) );
		}

		// delete old storage and set the members
		if( isOwning() )
		{
			delete[] pMemory_m;
		}
		pMemory_m = pNewMemory;
		length_m  = newLength;
	}
}


template<class TYPE>
void Array<TYPE>::copyObjects
(
	TYPE*const       pDestination,
	const TYPE*const pSource,
	const dword      length
)
{
	if( length >= 0 )
	{
		      TYPE* pDestinationCursor = pDestination + length;
		const TYPE* pSourceCursor      = pSource      + length;

		while( pDestinationCursor > pDestination )
		{
			*(--pDestinationCursor) = *(--pSourceCursor);
		}
	}
}


}//namespace




#endif//Array_h
