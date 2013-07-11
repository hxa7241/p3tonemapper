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


#ifdef _PLATFORM_WIN

#include <windows.h>   // kernel32.lib

#elif _PLATFORM_LINUX

#include <dlfcn.h>   // libdl.a

#endif

#include "DynamicLibraryInterface.hpp"   // own header is included last


using namespace hxa7241_general;




/// constants ------------------------------------------------------------------
static const char LIBRARY_LOAD_FAIL_MESSAGE[] =
	"dynamic library not found";
static const char FUNCTION_UNFOUND_MESSAGE[] =
	"dynamic library function not found";




/// ----------------------------------------------------------------------------
void hxa7241_general::loadLibrary
(
	const char libraryPathName[],
	void*&     libraryHandle
)
{
#ifdef _PLATFORM_WIN

	// loading and freeing a library wont work for some reason, so the library
	// is loaded first-time only in loadLibrary, and freeLibrary does nothing.

	if( 0 == libraryHandle )
	{
		// setting the search path is not supported on Win2000.

		// find name
		/*const char* pLibraryName = libraryPathName;
		{
			// loop forward thru pathname
			for( dword p = 0;  0 != libraryPathName[p];  ++p )
			{
				// set pos if char is path separator
				if( '\\' == libraryPathName[p] )
				{
					pLibraryName = libraryPathName + p + 1;
				}
			}
		}

		// add path to search list, (if there is a path (not just a name))
		if( pLibraryName > libraryPathName )
		{
			// copy path
			//std::vector<char> libraryPath( libraryPathName, pLibraryName );
			std::vector<char> libraryPath( pLibraryName - libraryPathName + 1 );
			for( udword p = 0;  p < libraryPath.size() - 1;  ++p )
			{
				libraryPath[p] = libraryPathName[p];
			}
			libraryPath.back() = 0;

			// add path to library search list
			::SetDllDirectory( &(libraryPath[0]) );
			// define the _WIN32_WINNT macro as 0x0502 in compilation
			// only in WinXP-sp1 or Win2003
		}*/

		// load library
		libraryHandle = reinterpret_cast<void*>( ::LoadLibraryEx(
			libraryPathName, 0, LOAD_WITH_ALTERED_SEARCH_PATH ) );
		if( 0 == libraryHandle )
		{
			//DWORD errorCode = ::GetLastError();
			//::FormatMessage(  );

			throw LIBRARY_LOAD_FAIL_MESSAGE;
		}
	}

#elif _PLATFORM_LINUX

	if( 0 != libraryPathName )
	{
		// load library
		libraryHandle = ::dlopen( libraryPathName, RTLD_LAZY | RTLD_GLOBAL );
		if( 0 == libraryHandle )
		{
			//const char* pErrorMessage = ::dlerror();
			::dlerror();

			throw LIBRARY_LOAD_FAIL_MESSAGE;
		}
	}
	else
	{
		libraryHandle = 0;

		throw LIBRARY_LOAD_FAIL_MESSAGE;
	}

#else

	libraryHandle = 0;
	throw LIBRARY_LOAD_FAIL_MESSAGE;

#endif
}


bool hxa7241_general::freeLibrary
(
#ifdef _PLATFORM_WIN
	void*& //libraryHandle
#else
	void*& libraryHandle
#endif
)
{
#ifdef _PLATFORM_WIN

	// loading and freeing a library wont work for some reason, so the library
	// is loaded first-time only in loadLibrary, and freeLibrary does nothing.

	return true;

//	const bool isOk =
//		(::FreeLibrary( reinterpret_cast<HMODULE>( libraryHandle ) ) != 0);
//	if( isOk )
//	{
//		libraryHandle = 0;
//	}
//
//	return isOk;

#elif _PLATFORM_LINUX

	if( 0 != libraryHandle )
	{
		if( 0 == ::dlclose( libraryHandle ) )
		{
			libraryHandle = 0;
		}
	}

	return 0 == libraryHandle;

#else

	return false;

#endif
}


FunctionPtr hxa7241_general::getFunction
(
	void*      libraryHandle,
	const char functionName[]
)
{
#ifdef _PLATFORM_WIN

	FARPROC pFunction = ::GetProcAddress(
		reinterpret_cast<HMODULE>( libraryHandle ), functionName );
	if( 0 == pFunction )
	{
		throw FUNCTION_UNFOUND_MESSAGE;
	}

	return reinterpret_cast<FunctionPtr>( pFunction );

#elif _PLATFORM_LINUX

	::dlerror();

	// reinterpret_cast doesnt always do the job
	FunctionPtr pFunction = (FunctionPtr)(
		::dlsym( libraryHandle, functionName ) );

	const char* pErrorMessage = ::dlerror();
	if( 0 != pErrorMessage )
	{
		throw FUNCTION_UNFOUND_MESSAGE;
	}

	return pFunction;

#else

	return reinterpret_cast<FunctionPtr>( 0 );

#endif
}
