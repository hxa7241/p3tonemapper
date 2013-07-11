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
#include "Foveal.hpp"

#include "Veil.hpp"   // own header is included last


using namespace p3tonemapper_tonemap;




/// statics --------------------------------------------------------------------
const float Veil::CENTRAL_WEIGHTING    = 0.913f;
const float Veil::PERIPHERAL_WEIGHTING = 1.0f - CENTRAL_WEIGHTING;




/// standard object services ---------------------------------------------------
Veil::Veil
(
   const Foveal& fovealImage
)
 : ImageRgbFloat()
{
   // set image base, same size and conversion
   ImageRgbFloat::setImage( fovealImage.getWidth(), fovealImage.getHeight() );
   ImageRgbFloat::setColorSpace( fovealImage.getColorSpace() );

   // fill pixels with convolution
   Veil::doBigConvolution( fovealImage, *this );
}


Veil::~Veil()
{
}


Veil::Veil
(
   const Veil& other
)
 : ImageRgbFloat( other )
{
}


Veil& Veil::operator=
(
   const Veil& other
)
{
   ImageRgbFloat::operator=( other );

   return *this;
}




/// commands -------------------------------------------------------------------




/// queries --------------------------------------------------------------------
void Veil::mixInto
(
   ImageRgbFloat& image
) const
{
   /**
    * visitor for Veil::mixInto.
    */
   class MixInto
      : public ImageRgbFloat::BilinearVisitor
   {
   public:
      MixInto( ImageRgbFloat& out )
       : pOut_m( &out )
      {
      }

      virtual void  operate( const hxa7241_graphics::Vector3f& in,
                             dword                             outX,
                             dword                             outY )
      {
         pOut_m->set( outX, outY,
            (pOut_m->get(outX, outY) *= CENTRAL_WEIGHTING) += in );
      }

      ImageRgbFloat* pOut_m;
   };


   // images same size (easy optimization)
   if( (getWidth()  == image.getWidth() ) &&
       (getHeight() == image.getHeight()) )
   {
      using hxa7241_graphics::Vector3f;

      // loop through pixels
      for( dword i = getLength();  i-- > 0; )
      {
         image.set( i, (image.get(i) *= CENTRAL_WEIGHTING) += get(i) );
         //*pOut = (*pOut * CENTRAL_WEIGHTING) + *pIn;
      }
   }
   else
   {
      MixInto visitor( image );
      ImageRgbFloat::visitBilinear( *this, visitor,
         image.getWidth(), image.getHeight() );
   }
}




/// implementation -------------------------------------------------------------
void Veil::doBigConvolution
(
   const Foveal& foveal,
   Veil&         veil
)
{
   // precondition: foveal and veil are same size and shape

   // (very literal translation from the equation, optimise later if needed...)

   // (this function appears to be O(n^4) but the input size is constrained to
   // less than about 120x120, so it is not *so* alarming)

   using hxa7241_graphics::Vector3f;
   static const float DEGREES_TO_RADIANS = 0.01745329251994f;

   // loop through veil pixels
   for( dword vy = veil.getHeight();  vy-- > 0; )
   {
      for( dword vx = veil.getWidth();  vx-- > 0; )
      {
         // zero weight sum
         // zero value sum
         float    weightSum = 0.0f;
         Vector3f valueSum;

         // loop through foveal pixels
         // to accumulate value
         for( dword fy = veil.getHeight();  fy-- > 0; )
         {
            for( dword fx = veil.getWidth();  fx-- > 0; )
            {
               // if pixel pos not same as veil
               if( !((fx == vx) & (fy == vy)) )
               {
                  // estimate angle between pixels
                  // (the foveal grid is defined as ~1 degree per pixel)
                  const float angle = DEGREES_TO_RADIANS * ::sqrtf(
                     float(((fx - vx) * (fx - vx)) + ((fy - vy) * (fy - vy))) );

                  // calc weight as: cos angle over angle squared
                  const float weight = ::cosf( angle ) / (angle * angle);

                  // add weight to weight sum
                  weightSum += weight;

                  // scale foveal pixel value by weight
                  const Vector3f veilContribution( weight *
                     foveal.get( fx, fy ) );

                  // add scaled value to value sum
                  valueSum += veilContribution;
               }
            }
         }

         // calc veil value
         // value sum over weight sum, multiplied by constant (complement of
         // constant in mixInto method)
         veil.set( vx, vy, (valueSum / weightSum) * PERIPHERAL_WEIGHTING );
      }
   }
}








/// test -----------------------------------------------------------------------
#ifdef TESTING


#include <iostream>


namespace p3tonemapper_tonemap
{
   using namespace hxa7241;


static bool testConstructor
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   seed
);
static bool testMixInto
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   seed
);


bool test_Veil
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   seed
)
{
   bool isOk = true;

   if( pOut ) *pOut << "[ test_Veil ]\n\n";

   isOk &= testConstructor( pOut, isVerbose, seed );
   isOk &= testMixInto( pOut, isVerbose, seed );

   if( pOut ) *pOut << (isOk ? "--- successfully" : "*** failurefully") <<
      " completed " << "\n\n\n";

   if( pOut ) pOut->flush();

   return isOk;
}


bool testConstructor
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   //seed
)
{
   using hxa7241_graphics::Vector3f;

   bool isOk = true;

   // flat -> flat
   {
      static const float PERIPHERAL_WEIGHTING = 0.087f;

      const Vector3f value( 128.0f, 64.0f, 32.0f );

      // make all same value foveal image
      ImageRgbFloat original( 80, 50 );
      for( dword i = original.getLength();  i-- > 0; )
      {
         original.set( i, value );
      }
      const Foveal foveal( original, 70.0f );

      // make veil from foveal
      const Veil veil( foveal );

      // measure veil pixel values
      bool isFail = false;
      {
         Vector3f min( Vector3f::MAX() );
         Vector3f max( Vector3f::MIN() );
         {
            for( dword i = veil.getLength();  i-- > 0; )
            {
               const Vector3f vi( veil.get(i) );
               const Vector3f dif( vi - (value * PERIPHERAL_WEIGHTING) );
               isFail |= dif.abs().largest() > 1e-5f;

               min.clampToMaxOf( vi );
               max.clampToMinOf( vi );
            }
         }

         isFail |= (max - min).abs().largest() >= 1e-3f;

         if( pOut && isVerbose ) *pOut << min << "  " << max << "  " <<
            (max - min).abs().largest() << "\n\n";
      }

      if( pOut ) *pOut << "flat-flat : " <<
         (!isFail ? "--- succeeded" : "*** failed") << "\n\n";
      isOk &= !isFail;
   }


   // point -> peak
   // (this is wrong -- the class doesnt produce a monotonic peak)
   /*{
      // make mid pixel foveal image
      const ImageRgbFloat original( 81, 51 );
      for( dword i = original.getLength();  i-- > 0; )
      {
         original.set( i, Vector3f::ZERO() );
      }
      original.get( original.getWidth() / 2, original.getHeight() / 2 ) =
         Vector3f( 1024.0f, 1024.0f, 1024.0f );
      const Foveal foveal( original, 70.0f );

      // make veil from foveal
      const Veil veil( foveal );

//    for( dword y = 0;  y < veil.getHeight();  ++y )
//    {
//       for( dword x = 0;  x < veil.getWidth();  ++x )
//       {
//          if( pOut && isVerbose ) *pOut << veil.get(x,y).getX() << " ";
//       }
//       if( pOut && isVerbose ) *pOut << "\n";
//    }

      bool isFail = false;

      const dword midX = veil.getWidth()  / 2;
      const dword midY = veil.getHeight() / 2;
      for( dword y = 0;  y < veil.getHeight();  ++y )
      {
         for( dword x = 0;  x < veil.getWidth();  ++x )
         {
            char is = ' ';

            // compare pixel with surrounding eight pixels
            for( dword yi = -1;  yi < 2;  ++yi )
            {
               for( dword xi = -1;  xi < 2;  ++xi )
               {
                  const dword xs = x + xi;
                  const dword ys = y + yi;
                  if( (xs > 0) & (xs < veil.getWidth()) &
                      (ys > 0) & (ys < veil.getHeight()) )
                  {
                     const dword dist2P = ((x - midX) * (x - midX)) +
                        ((y - midY) * (y - midY));
                     const dword dist2S = ((xs - midX) * (xs - midX)) +
                        ((ys - midY) * (ys - midY));
                     const Vector3f dif( veil.get(x,y) - veil.get(xs,ys) );

                     if( (xi == -1) & (yi == 0) )
                     {
                        if( ( (dist2P < dist2S) & (dif.sign() == Vector3f::ONE()) ) ||
                            ( (dist2P == dist2S) & (dif.sign() == Vector3f::ZERO()) ) ||
                            ( (dist2P > dist2S) & (dif.sign() == -Vector3f::ONE()) ) )
                        {
                           is = '1';
                        }
                        else
                        {
                           is = '0';
                        }
                        //is = dif.sign() == Vector3f::ONE() ? '+' : (dif.sign() == -Vector3f::ONE() ? '-' : '.');
                     }
                     //is &= (dist2P > dist2S) & (dif.sign() == -Vector3f::ONE());

                     // further should be smaller
                     isFail |= (dist2P < dist2S) & (dif.sign() !=  Vector3f::ONE());
                     isFail |= (dist2P > dist2S) & (dif.sign() != -Vector3f::ONE());
                  }
               }
            }

            if( pOut && isVerbose ) *pOut << is << " ";
         }
         if( pOut && isVerbose ) *pOut << "\n";
      }

      if( pOut ) *pOut << "point-peak : " <<
         (!isFail ? "--- succeeded" : "*** failed") << "\n\n";
      isOk &= !isFail;
   }*/


   return isOk;
}


bool testMixInto
(
   std::ostream* pOut,
   const bool    isVerbose,
   const dword   //seed
)
{
   using hxa7241_graphics::Vector3f;

   bool isOk = true;


   // test assignment and pixel operator only, since scaling is tested in
   // ImageRgbFloat::visitBilinear

   // flat veil mix into flat image
   {
      //static const float CENTRAL_WEIGHTING    = 0.913f;
      //static const float PERIPHERAL_WEIGHTING = 1.0f - CENTRAL_WEIGHTING;

      const Vector3f value1( 128.0f, 64.0f, 32.0f );

      // make all same value image and foveal
      ImageRgbFloat original( 311, 211 );
      for( dword i = original.getLength();  i-- > 0; )
      {
         original.set( i, value1 );
      }
      const Foveal foveal( original, 63.5f );

      // make veil from foveal
      const Veil veil( foveal );

      // mix veil back into original image
      veil.mixInto( original );

      // measure image pixel values
      bool isFail = false;
      Vector3f min( Vector3f::MAX() );
      Vector3f max( Vector3f::MIN() );
      for( dword i = veil.getLength();  i-- > 0; )
      {
         const Vector3f dif( original.get(i) - value1 );
         isFail |= dif.abs().largest() >= 1e-3f;

         min.clampToMaxOf( dif.abs() );
         max.clampToMinOf( dif.abs() );
      }

      if( pOut && isVerbose ) *pOut << min << "  " << max << "\n\n";


      if( pOut ) *pOut << "mix : " <<
         (!isFail ? "--- succeeded" : "*** failed") << "\n\n";
      isOk &= !isFail;
   }


   return isOk;
}


}//namespace


#endif//TESTING
