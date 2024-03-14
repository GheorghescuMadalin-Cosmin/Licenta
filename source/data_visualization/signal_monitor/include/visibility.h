#pragma once
/*
* The example bellow was extracted from https://gcc.gnu.org/wiki/Visibility
*/

#// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#ifndef CREATE_DLL
#define SIGNAL_MONITOR_LIB_IMPORT __declspec(dllimport)
#else
#define SIGNAL_MONITOR_LIB_EXPORT __declspec(dllexport)
#endif
#define SIGNAL_MONITOR_LIB_LOCAL
#else
#if __GNUC__ >= 4
#define SIGNAL_MONITOR_LIB_IMPORT __attribute__ ((visibility ("default")))
#define SIGNAL_MONITOR_LIB_EXPORT __attribute__ ((visibility ("default")))
#define SIGNAL_MONITOR_LIB_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define SIGNAL_MONITOR_LIB_IMPORT
#define SIGNAL_MONITOR_LIB_EXPORT
#define SIGNAL_MONITOR_LIB_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define SIGNAL_MONITOR_API and SIGNAL_MONITOR_LOCAL.
// SIGNAL_MONITOR_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// SIGNAL_MONITOR_LOCAL is used for non-api symbols.

#ifdef SIGNAL_MONITOR_DLL // defined if LIBRARY is compiled as a DLL
#ifdef SIGNAL_MONITOR_LIB_EXPORT // defined if we are building the LIBRARY DLL (instead of using it)
#define SIGNAL_MONITOR_API SIGNAL_MONITOR_LIB_EXPORT
#else
#define SIGNAL_MONITOR_API SIGNAL_MONITOR_LIB_IMPORT
#endif
#define SIGNAL_MONITOR_LOCAL SIGNAL_MONITOR_LIB_LOCAL
#else // LIBRARY_DLL is not defined: this means LIBRARY is a static lib.
#define SIGNAL_MONITOR_API
#define SIGNAL_MONITOR_LOCAL
#endif