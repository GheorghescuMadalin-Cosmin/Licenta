#pragma once
/*
* The example bellow was extracted from https://gcc.gnu.org/wiki/Visibility
*/

#// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#ifndef CREATE_DLL
#define WEBCAM_DATA_SOURCE_IMPORT __declspec(dllimport)
#else
#define WEBCAM_DATA_SOURCE_EXPORT __declspec(dllexport)
#endif
#define WEBCAM_DATA_SOURCE_LOCAL
#else
#if __GNUC__ >= 4
#define WEBCAM_DATA_SOURCE_IMPORT __attribute__ ((visibility ("default")))
#define WEBCAM_DATA_SOURCE_EXPORT __attribute__ ((visibility ("default")))
#define WEBCAM_DATA_SOURCE_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define WEBCAM_DATA_SOURCE_IMPORT
#define WEBCAM_DATA_SOURCE_EXPORT
#define WEBCAM_DATA_SOURCE_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define DUMMY_API and DUMMY_LOCAL.
// DUMMY_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// DUMMY_LOCAL is used for non-api symbols.

#ifdef WEBCAM_DATA_SOURCE_DLL // defined if LIBRARY is compiled as a DLL
#ifdef WEBCAM_DATA_SOURCE_EXPORT // defined if we are building the LIBRARY DLL (instead of using it)
#define WEBCAM_DATA_SOURCE_API WEBCAM_DATA_SOURCE_EXPORT
#else
#define WEBCAM_DATA_SOURCE_API WEBCAM_DATA_SOURCE_IMPORT
#endif
#define WEBCAM_LOCAL WEBCAM_DATA_SOURCE_LOCAL
#else // LIBRARY_DLL is not defined: this means LIBRARY is a static lib.
#define WEBCAM_DATA_SOURCE_API
#define WEBCAM_LOCAL
#endif