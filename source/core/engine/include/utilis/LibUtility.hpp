
#pragma once
#ifndef _WIN32
#include <dlfcn.h>
#include <gnu/lib-names.h>
#else
#include <windows.h>
#endif

#include <vector>
#include <mutex>
#include <defs/Log.hpp>
#include <defs/MdsInterface.hpp>

namespace core
{
    namespace utility
    {
        /*!
        *   @brief Lib loader facade
        */
        class LibUtility
        {
            std::vector<void*> handleContainer_; 
            LoggingInterface* logger_;
        public:
            /*!
            *   @brief Lib loader constructor
            */
            LibUtility(LoggingInterface* logger):
                logger_(logger)
            {

            }

            /*!
            *   @brief Method receive a library name and will try to load and resolve the symbols.
            *   @param libName path to the library that will be loaded. The path must include the library name.
            *   @param libSymbols library simbol name
            *   @return a pointer to library function
            *   @note The library will be closed when the LibUtility object is destroyed.
            */
            void* openLibrary(const std::string& libName, const char* libSymbols)
            {
                void* handle = nullptr;
#ifndef _WIN32
                handle = dlopen(libName.c_str(), RTLD_NOW);

                if(!handle)
                {
                    logger_->log(dlerror(), FACTORY_HANDLE, severity::debug);
                    return nullptr;
                }
#else
                handle = LoadLibrary(libName.c_str());
                if (!handle)
                {
                    logger_->log("Failed to load library!", FACTORY_HANDLE, severity::debug);
                    return nullptr;
                }
#endif
                handleContainer_.push_back(handle);
#ifndef _WIN32
                return dlsym(handle, libSymbols);
#else
                return GetProcAddress((HMODULE)handle, libSymbols);
#endif     
            }

            /*!
            *   @brief Method used to close the open library handles.
            *   @param libHandle library handle
            *   @return return 0 if the library was closed successfully
            */
            static int closeLibrary(void* libHandle)
            {
#ifndef _WIN32
                return dlclose(libHandle);
#else
                return FreeLibrary((HMODULE)libHandle);
#endif
            }
        };
    }
} // namespace core

