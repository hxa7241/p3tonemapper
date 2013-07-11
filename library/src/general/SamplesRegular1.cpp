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

#include "SamplesRegular1.hpp"   /// own header is included last


using namespace hxa7241_general;




/// standard object services ---------------------------------------------------
SamplesRegular1::SamplesRegular1()
 :	values_m()
 ,	xAxis_m ()
{
}


SamplesRegular1::SamplesRegular1
(
	const dword     samplesCount,
	const Interval& xAxis
)
 :	values_m()
 ,	xAxis_m ( xAxis )
{
	SamplesRegular1::setSize( samplesCount );
}


SamplesRegular1::~SamplesRegular1()
{
}


SamplesRegular1::SamplesRegular1
(
	const SamplesRegular1& other
)
{
	SamplesRegular1::operator=( other );
}


SamplesRegular1& SamplesRegular1::operator=
(
	const SamplesRegular1& other
)
{
	if( &other != this )
	{
		values_m = other.values_m;
		xAxis_m  = other.xAxis_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void SamplesRegular1::setSize
(
	const dword noOfSamples
)
{
	values_m.setLength( noOfSamples );
	values_m.zeroMemory();
}


void SamplesRegular1::setXAxis
(
	const Interval& xAxis
)
{
	xAxis_m = xAxis;
}


void SamplesRegular1::setYAxis
(
	const float scaling,
	const float offset
)
{
	float* pVal = values_m.getMemory();
	float* pEnd = pVal + values_m.getLength();
	for( ;  pVal < pEnd;  ++pVal )
	{
		*pVal = (*pVal * scaling) + offset;
	}
}


float SamplesRegular1::setCumulative
(
	const Array<float>& histogram,
	const Interval&     xAxis
)
{
	// the samples are at the points between, and around, the histogram bins,
	// so there is one more
	setSize( histogram.getLength() + 1 );
	setXAxis( xAxis );

	// make cumulative function from histogram
	// first element is zero, last (the extra one) is total
	const float* pCount = histogram.getMemory();
	const float* pEnd   = pCount + histogram.getLength();
			float* pSum   = values_m.getMemory();
	float sum = 0.0f;
	for( ;  pCount < pEnd;  ++pCount, ++pSum )
	{
		*pSum = sum;
		sum += *pCount;
	}
	*pSum = sum;

	return sum;
}


float SamplesRegular1::setCumulative
(
	const Array<float>& histogram,
	const Interval&     xAxis,
	const Interval&     yAxis
)
{
	const float total = setCumulative( histogram, xAxis );

	if( (1 < getSize()) && (0.0f < total) )
	{
		const float scaling = yAxis.getRange() / total;
		const float offset  = yAxis.getLower();

		setYAxis( scaling, offset );
	}
	else
	{
		setAll( yAxis.getLower() );
	}

	return total;
}


void SamplesRegular1::setAll
(
	const float value
)
{
	float* pVal = values_m.getMemory();
	float* pEnd = pVal + values_m.getLength();
	for( ;  pVal < pEnd;  ++pVal )
	{
		*pVal = value;
	}
}




/// queries --------------------------------------------------------------------
dword SamplesRegular1::getSize() const
{
	return values_m.getLength();
}


Interval SamplesRegular1::getXAxis() const
{
	return xAxis_m;
}


float SamplesRegular1::getValue
(
	const float xValue
) const
{
	float yValue = 0.0f;

	if( 1 < values_m.getLength() )
	{
		// calc fp point along array
		const float indexFp = float(values_m.getLength() - 1) *
			xAxis_m.getInterpolantClamped( xValue );

		// find adjacent indexs
		// Each element holds the y value for the x axis point: float(index).
		// There is an extra element for the point just off the top of the
		// (open) interval of the x axis -- so the highest interpolation
		// has a top bound.
		dword indexLower = dword( indexFp );
		indexLower       = indexLower <= (values_m.getLength() - 2) ?
			indexLower : (values_m.getLength() - 2);
		const dword indexUpper    = indexLower + 1;
		const float indexFraction = indexFp - float(indexLower);

		// interpolate value (linearly)
		yValue = values_m[indexLower] +
			((values_m[indexUpper] - values_m[indexLower]) *
			indexFraction);
	}
	else if( 1 == values_m.getLength() )
	{
		yValue = values_m[0];
	}

	return yValue;
}


const Array<float>& SamplesRegular1::getSamples() const
{
	return values_m;
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


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


bool test_SamplesRegular1
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	bool isOk = true;

	if( pOut ) *pOut << "[ test_SamplesRegular1 ]\n\n";


	// basics
	{
		bool isOk_ = true;

		{
			const SamplesRegular1 h1;
			const Interval x( h1.getXAxis() );
			const dword    s = h1.getSize();
			isOk_ &= (Interval::ZERO() == x) & (0 == s);

			if( pOut && isVerbose ) *pOut << x << "  " << s << "  " << isOk_ << "\n";
		}
		{
			static const float a = -22.5f;
			static const float b = 1138.0f;
			static const dword c = 3;

			const SamplesRegular1 h2( c, Interval(a,b) );
			const Interval x( h2.getXAxis() );
			const dword    s = h2.getSize();
			isOk_ &= (Interval(a,b) == x) & (c == s);

			if( pOut && isVerbose ) *pOut << x << "  " << s << "  " << isOk_ << "\n";
		}
		{
			static const float a = -22.5f;
			static const float b = 1138.0f;
			static const dword c = 3;

			const SamplesRegular1 h2( c, Interval(a,b) );
			const SamplesRegular1 h3( h2 );
			SamplesRegular1 h4;
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


	// initialisation
	{
		bool isOk_ = true;

		static const dword c = 2;
		static const dword d = 3;

		const SamplesRegular1 h1;
		const SamplesRegular1 h2( c, Interval() );
		SamplesRegular1 h3;
		h3.setSize( d );

		const SamplesRegular1* hs[] = { &h1, &h2, &h3 };
		for( udword i = 0;  i < sizeof(hs)/sizeof(hs[0]);  ++i )
		{
			const Array<float>& b = hs[i]->getSamples();
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
		static const float s = 5.0f;
		static const float o = 71.75;

		SamplesRegular1 h1;
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
			h1.setAll( d );

			h1.setYAxis( s, o );
			const Array<float>& b1 = h1.getSamples();
			for( dword j = b1.getLength();  j-- > 0; )
			{
				isOk_ &= ((d * s) + o) == b1[ j ];
			}

			if( pOut && isVerbose ) *pOut << b1 << "  " << isOk_ << "\n";
		}
		{
			Array<float> h( c );
			for( dword i = 0;  i < c;  ++i )
			{
				h[ i ] = float((c - i) + 10);
			}

			const float u = h1.setCumulative( h, Interval(a, b) );

			const Interval i1( h1.getXAxis() );
			isOk_ &= Interval(a, b) == i1;
			const dword s1 = h1.getSize();
			isOk_ &= (h.getLength() + 1) == s1;

			const Array<float>& b1 = h1.getSamples();
			float sum = 0.0f;
			for( dword i = 0;  i < c;  ++i )
			{
				isOk_ &= b1[i] == sum;
				sum += h[i];
			}
			isOk_ &= b1[b1.getLength() - 1] == sum;
			isOk_ &= u == sum;

			if( pOut && isVerbose ) *pOut << i1 << "  " << s1 << "  " << u <<
				"  " << b1 << "  " << isOk_ << "\n";
		}
		{
			h1.setAll( d );
			const Array<float>& b1 = h1.getSamples();
			for( dword j = b1.getLength();  j-- > 0; )
			{
				isOk_ &= d == b1[ j ];
			}

			if( pOut && isVerbose ) *pOut << b1 << "  " << isOk_ << "\n";
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

		SamplesRegular1 h1;
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
			h1.setSize( 0 );
			const float v1 = h1.getValue( 1.0f );
			isOk_ &= 0.0f == v1;

			Array<float> h( c );
			h[0] = 1.0f;
			h[1] = 3.0f;
			h[2] = 4.0f;
			const float s = h1.setCumulative( h, Interval(a, b) );

			const float v2 = h1.getValue( -10000.0f );
			const float v3 = h1.getValue(  10000.0f );
			isOk_ &= 0.0f == v2;
			isOk_ &= s == v3;

			if( pOut && isVerbose ) *pOut << v1 << "  " << v2 << "  " << v3 <<
				"  " << isOk_ << "\n";

			const Array<float>& b1 = h1.getSamples();
			const Interval i1( h1.getXAxis() );
			Array<float> vs( c );
			Array<float> ps( c );
			const float n = i1.getRange() / float(c);
			float p = i1.getLower() + (n / 2.0f);
			for( dword i = 0;  i < c;  ++i, p += n )
			{
				ps[i] = p;
				vs[i] = h1.getValue( p );

				isOk_ &= (b1[i] < vs[i]) & (b1[i + 1] > vs[i]);
			}

			if( pOut && isVerbose ) *pOut << b1 << "  " << ps << "  " << vs <<
				"  " << isOk_ << "\n";
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
