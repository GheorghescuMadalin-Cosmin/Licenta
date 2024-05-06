#pragma once
#include <defs/SignalExtraction.hpp>
#include <defs/MdsInterface.hpp>
#include <defs/MeasurementObjectDefs.hpp>

namespace core
{
    namespace signal
    {
        class SignalExtraction :
            public InterfaceAccess,
            public SignalExtractionInterface

        {
            InterfaceAccess* interfaceAccess_;
            bool extractWebCamData(void* payload, uint64_t handle, SignalGroup& outSignalMap);
            bool extractDataPayload(void* pkg, uint64_t handle, SignalGroup& outSignalMap);
        public:
            explicit SignalExtraction(InterfaceAccess* interfaceAccess);
            void* getInterface(const std::string& interfaceName) override;
            bool extract(DataPackageCPtr ptr, SignalGroup& outSignalMap) override;
        };
    }
}