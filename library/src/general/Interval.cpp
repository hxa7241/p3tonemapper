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


#include "Clamps.hpp"

#include "Interval.hpp"   /// own header is included last


using namespace hxa7241_general;




/// standard object services ---------------------------------------------------
Interval::Interval()
 : lower_m( 0.0f )
 , upper_m( 0.0f )
{
}


Interval::Interval
(
   const float lower,
   const float upper
)
{
   Interval::set( lower, upper );
}


Interval::Interval
(
   const Interval& other
)
{
   Interval::operator=( other );
}


Interval& Interval::operator=
(
   const Interval& other
)
{
   if( &other != this )
   {
      lower_m = other.lower_m;
      upper_m = other.upper_m;
   }

   return *this;
}




/// commands -------------------------------------------------------------------
void Interval::set
(
   const float lower,
   const float upper
)
{
   lower_m = lower;
   upper_m = upper;
}


void Interval::setLower
(
   const float lower
)
{
   lower_m = lower;
}


void Interval::setUpper
(
   const float upper
)
{
   upper_m = upper;
}


void Interval::ratchetMinMax
(
   const float value
)
{
   if( lower_m > value )
   {
      lower_m = value;
   }

   if( upper_m < value )
   {
      upper_m = value;
   }
}




/// queries --------------------------------------------------------------------
void Interval::get
(
   float& lower,
   float& upper
) const
{
   lower = lower_m;
   upper = upper_m;
}


Interval Interval::getNegative() const
{
   return Interval( upper_m, lower_m );
}


float Interval::getRange() const
{
   return upper_m - lower_m;
}


float Interval::getRangeAbs() const
{
   const float range = upper_m - lower_m;
   return range < 0.0f ? -range : range;
}


float Interval::getInterpolant
(
   const float value
) const
{
   return (value - lower_m) / (upper_m - lower_m);
}


float Interval::getInterpolantSafe
(
   const float value
) const
{
   return isEmpty() ? 0.0f : getInterpolant( value );
}


float Interval::getInterpolantClamped
(
   const float value
) const
{
   return hxa7241_general::clamp01c( getInterpolantSafe( value ) );
}


bool Interval::isInside
(
   const float value
) const
{
   return ((value >= lower_m) & (value <= upper_m)) ||
          ((value >= upper_m) & (value <= lower_m));
}


float Interval::getSign() const
{
   float sign = 0.0f;

   const float range = upper_m - lower_m;
   if( 0.0f < range )
   {
      sign = +1.0f;
   }
   else if( 0.0f > range )
   {
      sign = -1.0f;
   }

   return sign;
}


bool Interval::isEmpty() const
{
   return lower_m == upper_m;
}


bool Interval::isPositive() const
{
   return upper_m > lower_m;
}


bool Interval::isNegative() const
{
   return upper_m < lower_m;
}


bool Interval::operator==
(
   const Interval& other
) const
{
   return (lower_m == other.lower_m) &
          (upper_m == other.upper_m);
}


bool Interval::operator<
(
   const Interval& other
) const
{
   return getRangeAbs() < other.getRangeAbs();
}


bool Interval::operator>
(
   const Interval& other
) const
{
   return getRangeAbs() > other.getRangeAbs();
}




/// constants ------------------------------------------------------------------
const Interval& Interval::ZERO()
{
   static const Interval k;
   return k;
}


const Interval& Interval::ZEROTOONE()
{
   static const Interval k( 0.0f, 1.0f );
   return k;
}


const Interval& Interval::UNIT()
{
   static const Interval k( 0.0f, 1.0f );
   return k;
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


bool test_Interval
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   //seed
)
{
   bool isOk = true;

   if( pOut ) *pOut << "[ test_Interval ]\n\n";


   // basics
   {
      bool isOk_ = true;

      static const float a = -22.5f;
      static const float b = 1138.0f;

      Interval i1;
      isOk_ &= (0.0f == i1.getLower()) & (0.0f == i1.getUpper());
      if( pOut && isVerbose ) *pOut << i1 << "  " << isOk_ << "\n";

      Interval i2( a, b );
      isOk_ &= (a == i2.getLower()) & (b == i2.getUpper());
      if( pOut && isVerbose ) *pOut << i2 << "  " << isOk_ << "\n";

      Interval i3( i2 );
      isOk_ &= (a == i3.getLower()) & (b == i3.getUpper());
      if( pOut && isVerbose ) *pOut << i3 << "  " << isOk_ << "\n";

      Interval i4;
      i4 = i2;
      isOk_ &= (a == i4.getLower()) & (b == i4.getUpper());
      if( pOut && isVerbose ) *pOut << i4 << "  " << isOk_ << "\n";

      if( pOut && isVerbose ) *pOut << "\n";

      if( pOut ) *pOut << "basics : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   // commands
   {
      bool isOk_ = true;

      static const float a = -22.5f;
      static const float b = 1138.0f;

      {
         Interval i1;
         i1.set( a, b );
         isOk_ &= (a == i1.getLower()) & (b == i1.getUpper());

         if( pOut && isVerbose ) *pOut << i1 << "  " << isOk_ << "\n";
      }
      {
         Interval i2;
         i2.setLower( a );
         i2.setUpper( b );
         isOk_ &= (a == i2.getLower()) & (b == i2.getUpper());

         if( pOut && isVerbose ) *pOut << i2 << "  " << isOk_ << "\n";

         if( pOut && isVerbose ) *pOut << "\n";
      }
      {
         Interval i3( FLOAT_MAX, FLOAT_MIN_NEG );
         i3.ratchetMinMax( a );
         isOk_ &= (a == i3.getLower()) & (a == i3.getUpper());
         if( pOut && isVerbose ) *pOut << i3 << "  " << isOk_ << "\n";

         i3.ratchetMinMax( b );
         isOk_ &= (a == i3.getLower()) & (b == i3.getUpper());
         if( pOut && isVerbose ) *pOut << i3 << "  " << isOk_ << "\n";

         i3.ratchetMinMax( 0.0f );
         isOk_ &= (a == i3.getLower()) & (b == i3.getUpper());
         if( pOut && isVerbose ) *pOut << i3 << "  " << isOk_ << "\n";

         if( pOut && isVerbose ) *pOut << "\n";
      }

      if( pOut ) *pOut << "commands : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   // queries 1
   {
      bool isOk_ = true;

      static const float a = -22.5f;
      static const float b = 1138.0f;
      static const float c = 42.125f;

      {
         Interval i1( a, b );
         i1 = i1.getNegative();
         isOk_ &= (b == i1.getLower()) & (a == i1.getUpper());

         if( pOut && isVerbose ) *pOut << i1 << "  " << isOk_ << "\n";
      }
      {
         Interval i2( b, a );
         const float r1 = i2.getRange();
         const float r2 = i2.getRangeAbs();
         isOk_ &= (r1 == (a - b)) & (r2 == (b - a));

         if( pOut && isVerbose ) *pOut << i2 << "  " << r1 << "  " << r2 <<
            "  " << isOk_ << "\n";
      }
      {
         Interval i3( a, b );
         const float n1 = i3.getInterpolant( c );
         isOk_ &= ::fabsf(n1 - ((c - a) / (b - a))) < 1e-5f;
         const float n1a = i3.getInterpolantSafe( c );
         isOk_ &= ::fabsf(n1a - n1) < 1e-5f;
         const float n1b = i3.getInterpolantClamped( c );
         isOk_ &= ::fabsf(n1b - n1) < 1e-5f;

         if( pOut && isVerbose ) *pOut << i3 << "  " << n1 << "  " << n1a <<
            "  " << n1b << "  " << isOk_ << "\n";
      }
      {
         Interval i4( b, b );
         const float n2 = i4.getInterpolantSafe( c );
         isOk_ &= (n2 == 0.0f);
         const float n2a = i4.getInterpolantClamped( c );
         isOk_ &= (n2a == 0.0f);

         if( pOut && isVerbose ) *pOut << i4 << "  " << n2 << "  " << n2a <<
            "  " << isOk_ << "\n";
      }
      {
         static const float d = 100000.0f;
         static const float e = -100000.0f;
         Interval i5( a, b );
         const float n3 = i5.getInterpolantClamped( d );
         isOk_ &= (n3 == 1.0f);
         const float n4 = i5.getInterpolantClamped( e );
         isOk_ &= (n4 == 0.0f);

         if( pOut && isVerbose ) *pOut << i5 << "  " << n3 << "  " << n4 <<
            "  " << isOk_ << "\n";
      }
      if( pOut && isVerbose ) *pOut << "\n";

      if( pOut ) *pOut << "queries 1 : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   // queries 2
   {
      bool isOk_ = true;

      static const float a = -22.5f;
      static const float b = 1138.0f;

      {
         Interval i1( a, b );
         static const float ts[] = { 42.125f, 100000.0f, -100000.0f, a, b };
         static const bool  bs[] = { true, false, false, true, true };
         for( udword i = 0;  i < sizeof(ts)/sizeof(ts[0]);  ++i )
         {
            const bool s1 = i1.isInside( ts[i] );
            isOk_ &= (bs[i] == s1);

            const bool s2 = i1.getNegative().isInside( ts[i] );
            isOk_ &= (bs[i] == s2);

            if( pOut && isVerbose ) *pOut << i1 << "  " << ts[i] << "  " <<
               s1 << "  " << s2 << "  " << isOk_ << "\n";
         }

         if( pOut && isVerbose ) *pOut << "\n";
      }
      {
         const Interval is[] = {
            Interval( a, b ), Interval( b, a ), Interval( a, a ) };
         const float ss[] = { 1.0f,  -1.0f, 0.0f };
         const bool  es[] = { false, false, true };
         const bool  ps[] = { true,  false, false };
         const bool  ns[] = { false, true,  false };
         for( udword i = 0;  i < sizeof(is)/sizeof(is[0]);  ++i )
         {
            const float s = is[i].getSign();
            const bool  e = is[i].isEmpty();
            const bool  p = is[i].isPositive();
            const bool  n = is[i].isNegative();

            isOk_ &= (ss[i] == s);
            isOk_ &= (es[i] == e);
            isOk_ &= (ps[i] == p);
            isOk_ &= (ns[i] == n);

            if( pOut && isVerbose ) *pOut << is[i] << "  " << s << "  " <<
               e << "  " << p << "  " << n << "  " << isOk_ << "\n";
         }

         if( pOut && isVerbose ) *pOut << "\n";
      }

      if( pOut ) *pOut << "queries 2 : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   // queries 3
   {
      bool isOk_ = true;

      static const float a = -22.5f;
      static const float b = 1138.0f;

      {
         Interval i1( a, b );
         Interval i2( a, b );
         Interval i3( a, 0.0f );
         Interval i4;

         isOk_ &= (i1 == i2);
         if( pOut && isVerbose ) *pOut << i1 << "  " << i2 << "  " <<
            (i1 == i2) << "  " << isOk_ << "\n";

         isOk_ &= (i1 != i3);
         if( pOut && isVerbose ) *pOut << i1 << "  " << i2 << "  " <<
            (i1 != i3) << "  " << isOk_ << "\n";

         isOk_ &= (i1 != i4);
         if( pOut && isVerbose ) *pOut << i1 << "  " << i2 << "  " <<
            (i1 != i4) << "  " << isOk_ << "\n";

         if( pOut && isVerbose ) *pOut << "\n";
      }
      {
         Interval i1( a, b );

         const Interval is[] = { Interval( a, b ), Interval( b, a ),
            Interval( a - 10.0f, b ), Interval( a, b - 10.0f ),
            Interval( b + 10.0f, a ) };
         const bool ls[] = { false, false, true, false, true };
         const bool gs[] = { false, false, false, true, false };
         for( udword i = 0;  i < sizeof(is)/sizeof(is[0]);  ++i )
         {
            const bool l  = i1 <  is[i];
            const bool g  = i1 >  is[i];
            const bool le = i1 <= is[i];
            const bool ge = i1 >= is[i];

            isOk_ &= ( ls[i] == l );
            isOk_ &= ( gs[i] == g );
            isOk_ &= (!gs[i] == le);
            isOk_ &= (!ls[i] == ge);

            if( pOut && isVerbose ) *pOut << is[i] << "  " << l << "  " <<
               g << "  " << le << "  " << ge << "  " << isOk_ << "\n";
         }

         if( pOut && isVerbose ) *pOut << "\n";
      }

      if( pOut ) *pOut << "queries 3 : " <<
         (isOk_ ? "--- succeeded" : "*** failed") << "\n\n";

      isOk &= isOk_;
   }


   // constants
   {
      bool isOk_ = true;

      Interval i1( Interval::ZERO() );
      isOk_ &= (0.0f == i1.getLower()) & (0.0f == i1.getUpper());
      if( pOut && isVerbose ) *pOut << i1 << "  " << isOk_ << "\n";

      Interval i2( Interval::ZEROTOONE() );
      isOk_ &= (0.0f == i2.getLower()) & (1.0f == i2.getUpper());
      if( pOut && isVerbose ) *pOut << i2 << "  " << isOk_ << "\n";

      Interval i3( Interval::UNIT() );
      isOk_ &= (0.0f == i3.getLower()) & (1.0f == i3.getUpper());
      if( pOut && isVerbose ) *pOut << i3 << "  " << isOk_ << "\n";

      if( pOut && isVerbose ) *pOut << "\n";

      if( pOut ) *pOut << "constants : " <<
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
