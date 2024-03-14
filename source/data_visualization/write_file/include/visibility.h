#pragma once
/*
* The example bellow was extracted from https://gcc.gnu.org/wiki/Visibility
*/

#// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#ifndef CREATE_DLL
#define WRITE_LIB_IMPORT __declspec(dllimport)
#else
#define WRITE_LIB_EXPORT __declspec(dllexport)
#endif
#define WRITE_LIB_LOCAL
#else
#if __GNUC__ >= 4
#define WRITE_LIB_IMPORT __attribute__ ((visibility ("default")))
#define WRITE_LIB_EXPORT __attribute__ ((visibility ("default")))
#define WRITE_LIB_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define WRITE_LIB_IMPORT
#define WRITE_LIB_EXPORT
#define WRITE_LIB_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define WAVE_VIS_API and WAVE_VIS_LOCAL.
// WAVE_VIS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// WAVE_VIS_LOCAL is used for non-api symbols.

#ifdef WRITE_DLL // defined if LIBRARY is compiled as a DLL
#ifdef WRITE_LIB_EXPORT // defined if we are building the LIBRARY DLL (instead of using it)
#define WRITE_API WRITE_LIB_EXPORT
#else
#define WRITE_API WRITE_LIB_IMPORT
#endif
#define WRITE_LOCAL WRITE_LIB_LOCAL
#else // LIBRARY_DLL is not defined: this means LIBRARY is a static lib.
#define WRITE_API
#define WRITE_LOCAL
#endif