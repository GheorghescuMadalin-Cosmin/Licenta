#pragma once
/*
* The example bellow was extracted from https://gcc.gnu.org/wiki/Visibility
*/

//! Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#ifndef CREATE_DLL
#define CORE_LIB_IMPORT __declspec(dllimport)
#else
#define CORE_LIB_EXPORT __declspec(dllexport)
#endif
#define CORE_LIB_LOCAL
#else
#if __GNUC__ >= 4
#define CORE_LIB_IMPORT __attribute__ ((visibility ("default")))
#define CORE_LIB_EXPORT __attribute__ ((visibility ("default")))
#define CORE_LIB_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define CORE_LIB_IMPORT
#define CORE_LIB_EXPORT
#define CORE_LIB_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define CORE_API and CORE_LOCAL.
// CORE_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// CORE_LOCAL is used for non-api symbols.

#ifdef CORE_DLL // defined if LIBRARY is compiled as a DLL
#ifdef CORE_LIB_EXPORT // defined if we are building the LIBRARY DLL (instead of using it)
#define CORE_API CORE_LIB_EXPORT
#else
#define CORE_API CORE_LIB_IMPORT
#endif
#define CORE_LOCAL CORE_LIB_LOCAL
#else // LIBRARY_DLL is not defined: this means LIBRARY is a static lib.
#define CORE_API
#define CORE_LOCAL
#endif