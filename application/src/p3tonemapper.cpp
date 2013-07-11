/*--------------------------------------------------------------------

   Perceptuum3 rendering components
   Copyright (c) 2005-2007, Harrison Ainsworth / HXA7241.

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


#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>

#include "Primitives.hpp"

#include "ImageRef.hpp"
#include "ImageFormatter.hpp"

#include "p3tmPerceptualMap-v13.h"




using namespace hxa7241;
using std::string;
using std::vector;
using p3tonemapper_format::ImageFormatter;
using p3tonemapper_format::ImageRef;




/// constants ------------------------------------------------------------------
static const char HELP_MESSAGE[] =
"\n"
"----------------------------------------------------------------------\n"
"  P3 Tonemapper v1.3\n"
"\n"
"  Copyright (c) 2005-2007, Harrison Ainsworth / HXA7241.\n"
"  http://www.hxa7241.org/tonemapper/\n"
"\n"
"  2007-06-29\n"
"----------------------------------------------------------------------\n"
"\n"
#ifndef TESTING
"P3 Tonemapper transforms a high-dynamic-range image into a\n"
"low-dynamic-range image, in an automated way, sensitive to human\n"
"perception.\n"
"\n"
"usage:\n"
"  p3tonemapper {-help|-?}\n"
"  p3tonemapper [options] [-x:commandFilePathName] [-z] imageFilePathName\n"
"\n"
"options (with defaults shown):\n"
"  image formatting libraries:\n"
"   -lp:<string>    libpng pathname\n"
"   -le:<string>    openexr pathname\n"
"  image metadata:\n"
"   -ip:<8 floats>  primaries: 0.64_0.33_0.30_0.60_0.15_0.06_0.313_0.329\n"
"   -is:<2 floats>  pixel values scaling and offset: 1.0_0.0\n"
"   -iv:<1 float>   view angle horizontal degrees: 65.0\n"
"  mapping options:\n"
"   -m1:<htgca>     combination of sub-options (default is none):\n"
"     h               human (= all below)\n"
"     t               human tone adjustment\n"
"     g               human veiling glare (and tone)\n"
"     c               human color adjustment (and tone)\n"
"     a               human acuity filtering (and tone)\n"
"  output options:\n"
"   -or:<2 floats>  luminance range, max and min: 2.0_150.0\n"
"   -og:<float>     gamma transform to apply to output pixels: 0.45\n"
"   -ob:<8 | 16>    output pixel bits per channel: 8\n"
"  output file name:\n"
"   -on:<string>    output file path name <.png|.ppm>: inputFilePathName.png\n"
"\n"
"image file name must be last, and must end in '.exr' or '.hdr', '.pic',\n"
"'.rad', '.rgbe'.\n"
"\n"
"commandFilePathName defaults to 'p3tonemapper-opt.txt'\n"
"\n"
"-z switches on some feedback\n"
"\n"
"example:\n"
"  p3tonemapper somerendering.exr\n"
"  p3tonemapper -m1:g -or:1.4_100.0 -on:resultimage.png somerendering.hdr\n"
"\n";
#else//not TESTING
"*** TESTING BUILD ***\n"
"\n"
"usage:\n"
"   p3tonemapper [-t...] [-o...] [-s...]\n"
"\n"
"switches:\n"
"   -t<int>         which test: 1 to 15 for lib, -1 to -4 for app, 0 for all\n"
"   -o<0 | 1 | 2>   set output level: 0 = none, 1 = summaries, 2 = verbose\n"
"   -s<32bit int>   set random seed\n"
"\n";
#endif//TESTING
static const char SWITCH_HELP1[] = "?";
static const char SWITCH_HELP2[] = "help";

static const char COMMAND_FILE_NAME_DEFAULT[] = "p3tonemapper-opt.txt";

static const char EXCEPTION_PREFIX[]     = "*** execution failed:  ";
static const char EXCEPTION_ABSTRACT[]   = "no annotation for cause";
static const char WARNING_WRONG_SWITCH[] = "unrecognized option";




/// support declarations -------------------------------------------------------
#ifndef TESTING

class MapperWrapper
{
public:
   MapperWrapper()
    : pMapper_m( ::p3tmCreatePerceptualMapDefault() )
   {
   }

   ~MapperWrapper()
   {
      ::p3tmFreePerceptualMap( pMapper_m );
   }

   operator void* () { return pMapper_m; }

private:
   void* pMapper_m;

   MapperWrapper( const MapperWrapper& );
   MapperWrapper& operator=( const MapperWrapper& );
};


static void getInitialOptions
(
   const int      argc,
   char*const     argv[],
   bool&          isFeedback,
   string&        inImagePathname,
   vector<string> optionSets[2]
);


static void getOptions
(
   const vector<string> tokenSets[2],
   ImageFormatter*      pFormatter,
   void*                pMapper,
   dword*               pOutPixelType,
   string*              pOutPathname
);


static bool parseFps
(
   const string& group,
   float         fps[],
   const dword   length
);


static bool parseFp
(
   const string& s,
   float&        fp
);


static void tokenizeCommandLine
(
   const int       argc,
   char*const      argv[],
   vector<string>& tokens
);


static void tokenizeCommandFile
(
   const char*     commandFilePathname,
   vector<string>& tokens
);


static void tokenize
(
   const string&   str,
   const char      separator,
   vector<string>& tokens
);


static void printImageStats
(
   bool isFeedback,
   const ImageRef&
);


static void printMapper
(
   bool isFeedback,
   const void* pMapper
);


#else//not TESTING


static bool test
(
   int   argc,
   char* argv[]
);

static void readParameters
(
   const int  argc,
   char*const argv[],
   dword&     whichTest,
   dword&     outputLevel,
   dword&     seed
);

static bool testUnits
(
   int  whichTest,
   bool isOutput,
   bool isVerbose,
   int  seed
);

#endif//TESTING




/// entry point ////////////////////////////////////////////////////////////////
int main
(
   int   argc,
   char* argv[]
)
{
   int returnValue = EXIT_FAILURE;

   // catch everything
   try
   {
      // check for help request
      if( (argc <= 1) ||
         (('-' == argv[1][0]) &&
         ( (string(SWITCH_HELP1) == string(argv[1] + 1)) |
           (string(SWITCH_HELP2) == string(argv[1] + 1)) )) )
      {
         // output help
         std::cout << HELP_MESSAGE;

         returnValue = EXIT_SUCCESS;
      }
      // execute
      else
      {
#ifndef TESTING

         // make default mapper
         MapperWrapper mapper;

         // get input image pathname, and options
         bool           isFeedback;
         string         inImagePathname;
         vector<string> optionSets[2];
         getInitialOptions( argc, argv,
            isFeedback, inImagePathname, optionSets );

         // set formatter
         ImageFormatter formatter;
         getOptions( optionSets, &formatter, 0, 0, 0 );

         // read input image, and set mapper from its metadata
         ImageRef inImage;
         {
            // read image
            formatter.readImage( inImagePathname.c_str(), inImage );

            // set mapper from input image
            const float* pPrimaries8 = inImage.getPrimaries();
            if( 0 != pPrimaries8 )
            {
               ::p3tmSetInputColorSpace( mapper,
                  pPrimaries8, pPrimaries8 + 6 );
            }
            const float scaling = inImage.getScaling();
            if( 0.0f != scaling )
            {
               const float scalingOffset[2] = { scaling, 0.0f };
               ::p3tmSetInputLuminanceScale( mapper, scalingOffset );
            }

            printImageStats( isFeedback, inImage );
         }

         // get all other options, mostly into/overriding mapper
         string outImagePathname;
         dword  outImageType = 0;
         getOptions( optionSets, 0, mapper, &outImageType, &outImagePathname );

         // make output image
         ImageRef outImage( inImage, (::p3tm11_RGB_WORD == outImageType) ?
            ImageRef::PIXELS_WORD : ImageRef::PIXELS_BYTE );

         // call mapper to map
         {
            printMapper( isFeedback, mapper );

            char pMessage128[128] = "\0";
            bool isMapOk = 0 != ::p3tmMap( mapper,
               inImage.getWidth(), inImage.getHeight(), ::p3tm11_RGB_FLOAT,
               inImage.getPixels(), outImageType, outImage.getPixels(),
               pMessage128 );
            if( !isMapOk )
            {
               throw string( pMessage128 );
            }
         }

         // write output image
         {
            // maybe make default output image pathname
            if( outImagePathname.empty() )
            {
               const size_t extPos = inImagePathname.rfind( '.' );
               outImagePathname = inImagePathname.substr( 0, extPos ) + ".png";
            }

            if( isFeedback )
            {
               std::cout << "\noutput image pathname = " << outImagePathname << "\n";
            }

            formatter.writeImage( outImagePathname.c_str(), outImage );
         }

         // set return value
         returnValue = EXIT_SUCCESS;

#else//not TESTING

         returnValue = test( argc, argv ) ? EXIT_SUCCESS : EXIT_FAILURE;

#endif//TESTING
      }
   }
   // print exception message
   catch( const std::exception& e )
   {
      std::cout << '\n' << EXCEPTION_PREFIX << e.what() << '\n';
   }
   catch( const char*const pExceptionString )
   {
      std::cout << '\n' << EXCEPTION_PREFIX << pExceptionString << '\n';
   }
   catch( const std::string exceptionString )
   {
      std::cout << '\n' << EXCEPTION_PREFIX << exceptionString << '\n';
   }
   catch( ... )
   {
      std::cout << '\n' << EXCEPTION_PREFIX << EXCEPTION_ABSTRACT << '\n';
   }

   try
   {
      std::cout.flush();
   }
   catch( ... )
   {
      // suppress exceptions
   }

   return returnValue;
}




/// other functions ------------------------------------------------------------
#ifndef TESTING

void getInitialOptions
(
   const int      argc,
   char*const     argv[],
   bool&          isFeedback,
   string&        inImagePathname,
   vector<string> tokenSets[2]
)
{
   // read input file pathname from command line last arg
   inImagePathname = argv[argc - 1];

   // read command file pathname from command line
   string commandFilePathname( COMMAND_FILE_NAME_DEFAULT );
   for( int i = argc - 1;  i-- > 1; )
   {
      if( ('-' == argv[i][0]) && ('x' == argv[i][1]) && (':' == argv[i][2]) )
      {
         commandFilePathname = argv[i] + 3;
      }
   }

   // read feedback option
   isFeedback = false;
   for( int i = argc - 1;  i-- > 1; )
   {
      if( ('-' == argv[i][0]) && ('z' == argv[i][1]) )
      {
         isFeedback = true;
      }
   }

   // make command file token set
   tokenizeCommandFile( commandFilePathname.c_str(), tokenSets[0] );

   // make command line token set (except first and last)
   tokenizeCommandLine( argc - 2, argv + 1, tokenSets[1] );


// // print things
// std::cout << "\ninImagePathname     = " << inImagePathname;
// std::cout << "\ncommandFilePathname = " << commandFilePathname;
// std::cout << "\n";
//
// // print all tokens
// static const char* title[] = { "\ncommand file:", "\ncommand line:" };
// for( dword t = 0;  t < 2;  ++t )
// {
//    std::cout << title[t] << "\n\n";
//    for( udword i = 0;  i < tokenSets[t].size();  ++i )
//    {
//       std::cout << tokenSets[t][i] << "\n";
//    }
// }
}


void getOptions
(
   const vector<string> tokenSets[2],
   ImageFormatter*      pFormatter,
   void*                pMapper,
   dword*               pOutPixelType,
   string*              pOutPathname
)
{
   // get from command file, then override with command line
   for( dword i = 0;  i < 2;  ++i )
   {
      // select token set
      const vector<string>& tokens = tokenSets[i];

      // read switches from tokens
      for( int i = tokens.size();  i-- > 0; )
      {
         // eg: -lp:something_4.3
         const string& token = tokens[i];

         // only read if first char identifies a switch
         if( (token.length() >= 3) && ('-' == token[0]) )
         {
            const char   topKey = token[1];
            const char   subKey = token[2];
            const string value( token.substr( 4 ) );
            switch( topKey )
            {
            // libraries
            case 'l' :
               if( 0 != pFormatter )
               {
                  switch( subKey )
                  {
                  // libpng pathname
                  case 'p' :
                     pFormatter->setPngLibrary( value.c_str() );
                     break;

                  // openexr pathname
                  case 'e' :
                     pFormatter->setExrLibrary( value.c_str() );
                     break;
                  }
               }
               break;

            // image metadata
            case 'i' :
               if( 0 != pMapper )
               {
                  switch( subKey )
                  {
                  // colorspace primaries
                  case 'p' :
                     {
                        float colorspace[8] =
                           { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

                        parseFps( value, colorspace,
                           sizeof(colorspace)/sizeof(colorspace[0]) );

                        ::p3tmSetInputColorSpace(
                           pMapper, colorspace, colorspace + 6 );
                     }
                     break;

                  // calibration
                  case 's' :
                     {
                        float calibration[2] = { 0.0f, 0.0f };

                        parseFps( value, calibration,
                           sizeof(calibration)/sizeof(calibration[0]) );

                        ::p3tmSetInputLuminanceScale( pMapper, calibration );
                     }
                     break;

                  // view angle
                  case 'v' :
                     {
                        const float viewAngle = float(::atof( value.c_str() ));

                        ::p3tmSetInputViewAngle( pMapper, viewAngle );
                     }
                     break;
                  }
               }
               break;

            // mapping options
            case 'm' :
               if( ('1' == subKey) & (0 != pMapper) )
               {
                  dword mappingFlags = 0;

                  for( dword c = value.length();  c-- > 0; )
                  {
                     switch( value[c] )
                     {
                     case 'h' :
                        mappingFlags |= ::p3tm11_CONTRAST | ::p3tm11_GLARE |
                           ::p3tm11_COLOR | ::p3tm11_ACUITY;
                        break;

                     case 't' :
                        mappingFlags |= ::p3tm11_CONTRAST;
                        break;

                     case 'g' :
                        mappingFlags |= ::p3tm11_GLARE;
                        break;

                     case 'c' :
                        mappingFlags |= ::p3tm11_COLOR;
                        break;

                     case 'a' :
                        mappingFlags |= ::p3tm11_ACUITY;
                        break;
                     }
                  }

                  ::p3tmSetMappingFeatures( pMapper, mappingFlags );
               }
               break;

            // output options
            case 'o' :
               switch( subKey )
               {
               // out luminance range
               case 'r' :
                  if( 0 != pMapper )
                  {
                     float outLuminanceRange[2] = { 0.0f, 0.0f };

                     parseFps( value, outLuminanceRange,
                        sizeof(outLuminanceRange)/sizeof(outLuminanceRange[0]));

                     ::p3tmSetOutputLuminanceRange(
                        pMapper, outLuminanceRange );
                  }
                  break;

               // out gamma
               case 'g' :
                  if( 0 != pMapper )
                  {
                     const float outGamma = float(::atof( value.c_str() ));

                     ::p3tmSetOutputGamma( pMapper, outGamma );
                  }
                  break;

               // out pixel type
               case 'b' :
                  if( 0 != pOutPixelType )
                  {
                     const int i = ::atoi( value.c_str() );
                     *pOutPixelType = (16 == i) ?
                        ::p3tm11_RGB_WORD : ::p3tm11_RGB_BYTE;
                  }
                  break;

               // out image pathname
               case 'n' :
                  if( 0 != pOutPathname )
                  {
                     *pOutPathname = value;
                  }
                  break;
               }
               break;

            default  :
               ;//break;
            }
         }
      }

//    // print all args
//    std::cout << "   argc    = " << argc << '\n';
//    for( int i = 0;  i < argc;  ++i )
//    {
//       std::cout << "   argv[" << i << "] = " << argv[i] << '\n';
//    }
//    std::cout << '\n';
   }
}


bool parseFps
(
   const string& group,
   float         fps[],
   const dword   length
)
{
   vector<string> fpStrings;
   tokenize( group, '_', fpStrings );

   bool        isAllOk = (udword(length) == fpStrings.size());
   const dword minLength = length <= dword(fpStrings.size()) ?
      length : dword(fpStrings.size());
   for( dword f = minLength;  f-- > 0;  )
   {
      isAllOk &= parseFp( fpStrings[f], fps[f] );
   }

// // do all or nothing
// bool isAllOk = (length == fpStrings.size());
// for( udword f = 0;  (f < length) & isAllOk;  ++f )
// {
//    isAllOk &= parseFp( fpStrings[f], fps[f] );
// }

   return isAllOk;
}


bool parseFp
(
   const string& s,
   float&        fp
)
{
   errno = 0;

   const char* pNum = s.c_str();
   char*       pEnd = 0;

   fp = float( ::strtod( pNum, &pEnd ) );

   return (0 == errno) & (0 == *pEnd) & (pNum != pEnd);
}


void tokenizeCommandLine
(
   const int       argc,
   char*const      argv[],
   vector<string>& tokens
)
{
   for( dword i = 0;  i < argc;  ++i )
   {
      tokens.push_back( string(argv[i]) );
   }
}


void tokenizeCommandFile
(
   const char*     commandFilePathname,
   vector<string>& tokens
)
{
   std::ifstream commandFile( commandFilePathname );

   for( string token;  commandFile >> token; )
   {
      tokens.push_back( token );
   }
}


void tokenize
(
   const string&   str,
   const char      separator,
   vector<string>& tokens
)
{
   size_t posToken   = 0;
   bool   isSepFound = false;
   do
   {
      const size_t posSep = str.find( separator, posToken );
      isSepFound          = string::npos != posSep;

      tokens.push_back( str.substr( posToken,
         isSepFound ? posSep - posToken : string::npos ) );
      posToken = posSep + 1;
   }
   while( isSepFound );
}


//void tokenize
//(
// const string&   str,
// vector<string>& tokens
//)
//{
// std::stringstream ss( str );
//
// for( string token;  ss >> token; )
// {
//    tokens.push_back( token );
// }
//}


static void printImageStats
(
   const bool      isFeedback,
   const ImageRef& image
)
{
   if( isFeedback )
   {
      ImageRef::EPixelType pixelsType = image.getPixelType();
      if( ImageRef::PIXELS_FLOAT == pixelsType )
      {
         float min  = FLOAT_MAX;
         float max  = FLOAT_MIN_NEG;
         float mean = 0.0f;
         {
            const float  scaling = image.getScaling();
            const float* pPixels = static_cast<const float*>(
               image.getPixels() );
            const dword  length  = image.getWidth() * image.getHeight();

            for( dword i = 0;  i < length;  ++i, pPixels += 3 )
            {
               float luminance = (0.2126f * pPixels[0]) +
                  (0.7152f * pPixels[1]) + (0.0722f * pPixels[2]);
               luminance = (0.0f != scaling) ? luminance * scaling : luminance;

               min   = min <= luminance ? min : luminance;
               max   = max >= luminance ? max : luminance;
               mean += luminance;
            }

            mean /= (length > 0) ? float(length) : 1.0f;
         }

         std::cout << "\ninput image stats";
         std::cout << "\n   luminance min, max, mean = " << min << "  " <<
            max << "  " << mean;
         std::cout << "\n";
      }
      else
      {
         // not done yet...
      }
   }
}


void printMapper
(
   const bool  isFeedback,
   const void* pMapper
)
{
   if( isFeedback )
   {
      float inChromaticities6[6];
      float inWhitePoint2[2];
      float inScalingAndOffset2[2];
      float inViewAngleHorizontal;
      int   mappingFlags;
      float outLuminanceRange2[2];
      float outGamma;

      ::p3tmGetOptions( pMapper,
         inChromaticities6,
         inWhitePoint2,
         inScalingAndOffset2,
         &inViewAngleHorizontal,
         &mappingFlags,
         outLuminanceRange2,
         &outGamma );

      std::cout << "\nmapper fields";
      std::cout << "\n   inChromaticities      = ";
      for( dword i = 0;  i < 6;  ++i )
      {
         std::cout << inChromaticities6[i] << " ";
      }
      std::cout << "\n   inWhitePoint          = " << inWhitePoint2[0] <<
         " " << inWhitePoint2[1];
      std::cout << "\n   inScalingAndOffset    = " << inScalingAndOffset2[0] <<
         " " << inScalingAndOffset2[1];
      std::cout << "\n   inViewAngleHorizontal = " << inViewAngleHorizontal;
      std::cout << "\n   mappingFlags          = " <<
         ((mappingFlags & ::p3tm11_CONTRAST ) ? "t" : "") << " " <<
         ((mappingFlags & (::p3tm11_GLARE  & ~::p3tm11_CONTRAST)) ?
            "g" : "") << " " <<
         ((mappingFlags & (::p3tm11_COLOR  & ~::p3tm11_CONTRAST)) ?
            "c" : "") << " " <<
         ((mappingFlags & (::p3tm11_ACUITY & ~::p3tm11_CONTRAST)) ?
            "a" : "");
      std::cout << "\n   outLuminanceRange2    = " << outLuminanceRange2[0] <<
         " " << outLuminanceRange2[1];
      std::cout << "\n   outGamma              = " << outGamma;
      std::cout << "\n";
   }
}


#else//not TESTING


bool test
(
   int   argc,
   char* argv[]
)
{
   // default options
   dword whichTest = 0;
   bool  isVerbose = false;
   bool  isQuiet   = true;
   dword seed      = 0;

   // read options
   {
      dword outputLevel = 0;
      readParameters( argc, argv, whichTest, outputLevel, seed );
      isVerbose = (2 == outputLevel);
      isQuiet   = (0 == outputLevel);
   }

   // run tests
   bool isOk = true;
   if( whichTest <= 0 )
   {
      isOk &= testUnits( -whichTest, !isQuiet, isVerbose, seed );
   }
   std::cout.flush();
   if( whichTest >= 0 )
   {
      isOk &= (0 != ::p3tmTestUnits( whichTest, !isQuiet, isVerbose, seed ));
   }

   // print result
   if( !isQuiet )
   {
      std::cout <<
      (isOk ? "--- successfully" : "*** failurefully") << " completed " <<
      ((0 == whichTest) ? "all unit tests" : "one unit test") << "\n\n";
   }
   else
   {
      std::cout << isOk << "\n";
   }

   std::cout.flush();

   return isOk;
}


void readParameters
(
   const int  argc,
   char*const argv[],
   dword&     whichTest,
   dword&     outputLevel,
   dword&     seed
)
{
   // read switches from args
   for( int i = argc;  i-- > 1; )
   {
      // only read if first char identifies a switch
      const char first = argv[i][0];
      if( '-' == first )
      {
         const char       key    = argv[i][1];
         const char*const pValue = argv[i] + 2;
         switch( key )
         {
            // which test
            case 't' :
            {
               // read int, defaults to 0
               whichTest = std::atoi( pValue );

               break;
            }
            // output level
            case 'o' :
            {
               // read int, defaults to 0
               outputLevel = std::atoi( pValue );

               // clamp to range
               if( outputLevel < 0 )
               {
                  outputLevel = 0;
               }
               else if( outputLevel > 2 )
               {
                  outputLevel = 2;
               }

               break;
            }
            // seed
            case 's' :
            {
               // read int, defaults to 0
               seed = std::atoi( pValue );

               break;
            }
            default  :
               ;//break;
         }
      }
   }


   // print all args
   std::cout << '\n';
   std::cout << "   argc    = " << argc << '\n';
   for( int i = 0;  i < argc;  ++i )
   {
      std::cout << "   argv[" << i << "] = " << argv[i] << '\n';
   }

// // print parameters
// std::cout << '\n';
// std::cout << "   which test   = " << whichTest << '\n';
// std::cout << "   output level = " << outputLevel << '\n';
// std::cout << "   seed         = " << seed << '\n';
}


/// unit test declarations
namespace p3tonemapper_format
{
   namespace exr
   {
      bool test_exr ( std::ostream* pOut, bool isVerbose, dword seed );
   }

   namespace png
   {
      bool test_png ( std::ostream* pOut, bool isVerbose, dword seed );
   }

   namespace ppm
   {
      bool test_ppm ( std::ostream* pOut, bool isVerbose, dword seed );
   }

   namespace rgbe
   {
      bool test_rgbe( std::ostream* pOut, bool isVerbose, dword seed );
   }
}


/// unit test caller
static bool (*TESTERS[])(std::ostream*, bool, dword) =
{
   &p3tonemapper_format::exr::test_exr           // 1
,  &p3tonemapper_format::png::test_png           // 2
,  &p3tonemapper_format::ppm::test_ppm           // 3
,  &p3tonemapper_format::rgbe::test_rgbe         // 4
};


bool testUnits
(
   int        whichTest,
   const bool isOutput,
   const bool isVerbose,
   const int  seed
)
{
   bool isOk = true;

   if( isOutput ) std::cout << "\n\n";

   const dword noOfTests = sizeof(TESTERS)/sizeof(TESTERS[0]);
   if( 0 >= whichTest )
   {
      for( dword i = 0;  i < noOfTests;  ++i )
      {
         isOk &= (TESTERS[i])(
            isOutput ? &std::cout : 0, isVerbose, seed );
      }
   }
   else
   {
      if( whichTest > noOfTests )
      {
         whichTest = noOfTests;
      }
      isOk &= (TESTERS[whichTest - 1])(
         isOutput ? &std::cout : 0, isVerbose, seed );
   }

   if( isOutput ) std::cout <<
      (isOk ? "--- successfully" : "*** failurefully") << " completed " <<
      ((0 >= whichTest) ? "all app unit tests" : "one app unit test") << "\n\n";

   return isOk;
}

#endif//TESTING
