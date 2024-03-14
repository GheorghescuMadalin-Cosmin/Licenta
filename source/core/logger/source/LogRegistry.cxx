#include "pch.h"
#include <defs/MdsInterface.hpp>
#include <LogRegistry.hpp>
#include <string>

namespace core
{
    LogRegister::LogRegister(InterfaceAccess* interfaceAccess):
        log_(nullptr)
    {
        log_ = reinterpret_cast<LoggingInterface*>(interfaceAccess->getInterface("LoggingInterface"));
        register_[ENGINE_HANDLE] = "Engine";
        register_[LOGGER_HANDLE] = "Logger";
    }
    bool LogRegister::insertHandle(const uint64_t handle, const char* name)
    {
        auto iter = register_.find(handle);

        if(iter != register_.end())
        {
            log_->log("Handle already registered", handle, severity::warning);
            return false;
        }

        register_[handle] = name;
        log_->log("Registered new handle", handle, severity::information);
        return true;
    }
    bool LogRegister::removeHandle(const uint64_t handle)
    {
        auto iter = register_.find(handle);

        if(iter == register_.end())
        {
            log_->log("Handle not registered", handle, severity::warning);
            return false;
        }

        log_->log("Removed handle", handle, severity::information);
        register_.erase(iter);
        return true;
    }

    const char* LogRegister::getName(const uint64_t handle)
    {
        auto iter = register_.find(handle);

        if(iter == register_.end())
        {
            return std::to_string(handle).c_str();
        }

        return register_[handle];
    }
}