#pragma once
/*
* The example bellow was extracted from https://gcc.gnu.org/wiki/Visibility
*/

#// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#ifndef CREATE_DLL
#define XML_LIB_IMPORT __declspec(dllimport)
#else
#define XML_LIB_EXPORT __declspec(dllexport)
#endif
#define XML_LIB_LOCAL
#else
#if __GNUC__ >= 4
#define XML_LIB_IMPORT __attribute__ ((visibility ("default")))
#define XML_LIB_EXPORT __attribute__ ((visibility ("default")))
#define XML_LIB_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define XML_LIB_IMPORT
#define XML_LIB_EXPORT
#define XML_LIB_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define XML_API and XML_LOCAL.
// XML_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// XML_LOCAL is used for non-api symbols.

#ifdef XML_DLL // defined if LIBRARY is compiled as a DLL
#ifdef XML_LIB_EXPORT // defined if we are building the LIBRARY DLL (instead of using it)
#define XML_API XML_LIB_EXPORT
#else
#define XML_API XML_LIB_IMPORT
#endif
#define XML_LOCAL XML_LIB_LOCAL
#else // LIBRARY_DLL is not defined: this means LIBRARY is a static lib.
#define XML_API
#define XML_LOCAL
#endif