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


#include "Histogram.hpp"   /// own header is included last


using namespace hxa7241_general;




/// statics
static dword MAX_SIZE = dword(WORD_MAX);




/// standard object services ---------------------------------------------------
Histogram::Histogram()
 :	bins_m ()
 ,	xAxis_m( Interval::UNIT() )
{
	Histogram::setSize( 1 );
}


Histogram::Histogram
(
	const dword     noOfBins,
	const Interval& xAxis
)
 :	bins_m ()
 ,	xAxis_m( xAxis )
{
	Histogram::setSize( noOfBins );
}


Histogram::~Histogram()
{
}


Histogram::Histogram
(
	const Histogram& other
)
{
	Histogram::operator=( other );
}


Histogram& Histogram::operator=
(
	const Histogram& other
)
{
	if( &other != this )
	{
		bins_m  = other.bins_m;
		xAxis_m = other.xAxis_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void Histogram::setSize
(
	dword noOfBins
)
{
	if( 0 >= noOfBins )
	{
		noOfBins = 1;
	}
	else if( MAX_SIZE < noOfBins )
	{
		noOfBins = MAX_SIZE;
	}

	bins_m.setLength( noOfBins );
	bins_m.zeroMemory();
}


void Histogram::setXAxis
(
	const Interval& xAxis
)
{
	xAxis_m = xAxis;
}


void Histogram::setAllBins
(
	const float value
)
{
	float* pBin = bins_m.getMemory();
	float* pEnd = pBin + bins_m.getLength();
	for( ;  pBin < pEnd;  ++pBin )
	{
		*pBin = value;
	}
}


void Histogram::addToBin
(
	const float position,
	const float increment
)
{
	// only include if position within the x axis
	if( xAxis_m.isInside( position ) )
	{
		// find bin index
		dword bin = dword(
			xAxis_m.getInterpolantSafe( position ) *
			float(bins_m.getLength()) );

		// clamp bin index max
		if( bins_m.getLength() <= bin )
		{
			bin = bins_m.getLength() - 1;
		}

		// increment bin value
		bins_m[bin] += increment;
	}
}


Array<float>& Histogram::getBins()
{
	return bins_m;
}




/// queries --------------------------------------------------------------------
dword Histogram::getSize() const
{
	return bins_m.getLength();
}


Interval Histogram::getXAxis() const
{
	return xAxis_m;
}


float Histogram::getBinWidth() const
{
	return xAxis_m.getRange() / float(bins_m.getLength());
}


float Histogram::getTotal() const
{
	float total = 0.0f;

	const float* pBin = bins_m.getMemory();
	const float* pEnd = pBin + bins_m.getLength();
	for( ;  pBin < pEnd;  ++pBin )
	{
		total += *pBin;
	}

	return total;
}


const Array<float>& Histogram::getBins() const
{
	return bins_m;
}


dword Histogram::getMaxSize()
{
	return MAX_SIZE;
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <math.h>
#include <iostream>


namespace hxa7241_general
{
	using namespace hxa7241;


static std::ostream& operator<<
(
	std::ostream&   out,
	const Interval& i
)
{
	return out << "(" << i.getLower() << "," << i.getUpper() << ")";
}


static std::ostream& operator<<
(
	std::ostream&       out,
	const Array<float>& a
)
{
	out << "{";
	const dword e = a.getLength() - 1;
	for( dword i = 0;  i <= e;  ++i )
	{
		out << a[i] << (e != i ? ", " : "");
	}
	out << "}";

	return out;
}


bool test_Histogram
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_Histogram ]\n\n";


	// basics
	{
		bool isOk_ = true;

		{
			const Histogram h1;
			const Interval x( h1.getXAxis() );
			const dword    s = h1.getSize();
			isOk_ &= (Interval::UNIT() == x) & (1 == s);

			if( pOut && isVerbose ) *pOut << x << "  " << s << "  " << isOk_ << "\n";
		}
		{
			static const float a = -22.5f;
			static const float b = 1138.0f;
			static const dword c = 3;

			const Histogram h2( c, Interval(a,b) );
			const Interval x( h2.getXAxis() );
			const dword    s = h2.getSize();
			isOk_ &= (Interval(a,b) == x) & (c == s);

			if( pOut && isVerbose ) *pOut << x << "  " << s << "  " << isOk_ << "\n";
		}
		{
			static const float a = -22.5f;
			static const float b = 1138.0f;
			static const dword c = 3;

			const Histogram h2( c, Interval(a,b) );
			const Histogram h3( h2 );
			Histogram h4;
			h4 = h2;

			const Interval x3( h3.getXAxis() );
			const dword    s3 = h3.getSize();
			isOk_ &= (x3 == h2.getXAxis()) & (s3 == h2.getSize());

			if( pOut && isVerbose ) *pOut << x3 << "  " << s3 << "  " << isOk_ << "\n";

			const Interval x4( h4.getXAxis() );
			const dword    s4 = h4.getSize();
			isOk_ &= (x4 == h2.getXAxis()) & (s4 == h2.getSize());

			if( pOut && isVerbose ) *pOut << x4 << "  " << s4 << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "basics : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// invariants
	{
		bool isOk_ = true;

		static const dword c = 0;
		static const dword d = -11;

		const Histogram h1;
		const Histogram h2( c, Interval() );
		const Histogram h3( d, Interval() );
		const Histogram h4( h2 );
		Histogram h5;
		h5 = h3;

		const Histogram* hs[] = { &h1, &h2, &h3, &h4, &h5 };
		for( udword i = 0;  i < sizeof(hs)/sizeof(hs[0]);  ++i )
		{
			const dword s = hs[i]->getSize();
			isOk_ &= s > 0;

			const Array<float>& b = hs[i]->getBins();
			if( pOut && isVerbose ) *pOut << b << "  " << s << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "invariants : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// initialisation
	{
		bool isOk_ = true;

		static const dword c = 2;
		static const dword d = 3;

		const Histogram h1;
		const Histogram h2( c, Interval() );
		Histogram h3;
		h3.setSize( d );

		const Histogram* hs[] = { &h1, &h2, &h3 };
		for( udword i = 0;  i < sizeof(hs)/sizeof(hs[0]);  ++i )
		{
			const Array<float>& b = hs[i]->getBins();
			for( dword j = b.getLength();  j-- > 0; )
			{
				isOk_ &= 0.0f == b[ j ];
			}

			if( pOut && isVerbose ) *pOut << b << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "initialisation : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// commands
	{
		bool isOk_ = true;

		static const float a = -22.5f;
		static const float b = 1138.0f;
		static const dword c = 3;
		static const float d = 42.125f;

		Histogram h1;
		{
			h1.setSize( c );
			const dword s1 = h1.getSize();
			isOk_ &= c == s1;

			if( pOut && isVerbose ) *pOut << s1 << "  " << isOk_ << "\n";
		}
		{
			h1.setXAxis( Interval(a, b) );
			const Interval i1( h1.getXAxis() );
			isOk_ &= Interval(a, b) == i1;

			if( pOut && isVerbose ) *pOut << i1 << "  " << isOk_ << "\n";
		}
		{
			h1.setAllBins( d );
			const Array<float>& b1 = h1.getBins();
			for( dword j = b1.getLength();  j-- > 0; )
			{
				isOk_ &= d == b1[ j ];
			}

			if( pOut && isVerbose ) *pOut << b1 << "  " << isOk_ << "\n";
		}
		{
			static const float e = -7.333f;
			static const float f = 10.34811131f;
			static const dword g = 3;
			static const float is[] = { 0.25f, 1.0f, -0.5f };

			Histogram h2( g, Interval(e, f) );

			h2.addToBin( -100.0f, is[0] );
			h2.addToBin(  100.0f, is[0] );

			const Array<float>& b2 = h2.getBins();
			for( dword j = b2.getLength();  j-- > 0; )
			{
				isOk_ &= 0.0f == b2[ j ];
			}

			if( pOut && isVerbose ) *pOut << b2 << "  " << isOk_ << "\n";

			const float w = (f-e) / float(g);
			const dword v = 3;
			for( dword k = v;  k-- > 0; )
			{
				for( float p = e + (w / 4.0f);  p < f;  p += w )
				{
					h2.addToBin( p, is[k] );
				}
			}

			for( dword j = b2.getLength();  j-- > 0; )
			{
				isOk_ &= (is[0] + is[1] + is[2]) == b2[ j ];
			}

			if( pOut && isVerbose ) *pOut << b2 << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "commands : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	// queries
	{
		bool isOk_ = true;

		static const float a = -22.5f;
		static const float b = 1138.0f;
		static const dword c = 3;
		static const float d = 42.125f;

		Histogram h1;
		{
			h1.setSize( c );
			const dword s1 = h1.getSize();
			isOk_ &= c == s1;

			if( pOut && isVerbose ) *pOut << s1 << "  " << isOk_ << "\n";
		}
		{
			h1.setXAxis( Interval(a, b) );
			const Interval i1( h1.getXAxis() );
			isOk_ &= Interval(a, b) == i1;

			if( pOut && isVerbose ) *pOut << i1 << "  " << isOk_ << "\n";
		}
		{
			const float w = h1.getBinWidth();
			const float dif = ::fabsf( w - ((b - a) / float(c)) );
			isOk_ &= dif < 1e-4f;

			if( pOut && isVerbose ) *pOut << w << "  " << dif << "  " << isOk_ << "\n";
		}
		{
			h1.setAllBins( d );
			const float t = h1.getTotal();
			const float dif = ::fabsf( t - (d * float(h1.getSize())) );
			isOk_ &= dif < 1e-4f;

			if( pOut && isVerbose ) *pOut << t << "  " << dif << "  " << isOk_ << "\n";
		}

		if( pOut && isVerbose ) *pOut << "\n";

		if( pOut ) *pOut << "queries : " <<
			(isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

		isOk &= isOk_;
	}


	if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
		" completed " << "\n\n\n";

	if( pOut ) pOut->flush();


	return isOk;
}


}//namespace


#endif//TESTING
