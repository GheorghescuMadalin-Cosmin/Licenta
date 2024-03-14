#pragma once
#include <defs/MeasurementObjectDefs.hpp>
#include <unordered_map>
#include <cstdint>

namespace core
{
    namespace utils
    {
        //Function to generate a unique uint64_t handle
        static uint64_t generateHandle(uint8_t instance, MeasurementObjectType type) 
        {
            // Use a static variable to keep track of the offset for each type
            static std::unordered_map<MeasurementObjectType, uint64_t> typeOffsets;

            // If the offset for this type doesn't exist, create it
            if (typeOffsets.find(type) == typeOffsets.end()) {
                typeOffsets[type] = 0;
            }

            // Calculate the handle with the offset and increment the offset for the next time
            uint64_t handle = ((static_cast<uint64_t>(instance) << 40) | 
                            (static_cast<uint64_t>(static_cast<uint8_t>(type)) << 16) | 
                            static_cast<uint64_t>(type)) + typeOffsets[type];

            typeOffsets[type]++;

            return handle;
        }
    }
}