#pragma once
#include <map>
#include <string>
#include <cstdint>
#include <defs/DataPackage.hpp>

using SignalMessage = std::map<std::string, std::string>;
using SignalGroup = std::map<std::string, SignalMessage>;

struct SignalExtractionInterface
{
    virtual bool extract(DataPackageCPtr ptr, SignalGroup& outSignalMap) = 0;
};