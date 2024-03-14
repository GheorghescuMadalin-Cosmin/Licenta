#pragma once
#include <map>
#include <cstdint>
#include <defs/Log.hpp>
#include <defs/MdsInterface.hpp>

namespace core
{
    class LogRegister
    {
        std::map<uint64_t, const char*> register_;
        LoggingInterface* log_;
    public:
        LogRegister(InterfaceAccess* interfaceAccess);
        bool insertHandle(const uint64_t handle, const char* name);
        bool removeHandle(const uint64_t handle);
        const char* getName(const uint64_t handle);
    };
}