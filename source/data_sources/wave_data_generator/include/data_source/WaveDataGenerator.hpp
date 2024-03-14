#pragma once
#include <visibility.h>
#include <defs/MdsInterface.hpp>
#include <defs/Distribution.hpp>
#include <memory>
#include <defs/Distribution.hpp>
#include <defs/Transmitter.hpp>
#include <thread>
#include <mutex>

namespace data_source
{
    class WAVE_DATA_SOURCE_API WaveGenerator :
        public InterfaceAccess,
        public MeasurementObject,
        public DataSenderObject,
        public ObjectControl,
        public GuiControlIfc
    {
        DataDistributionPtr dataDistributinonPtr_;
        InterfaceAccess* interfaceAccess_;
        uint8_t instanceNumber_;
        uint64_t handle_;
        int samplePoints_;
        std::string name_;
        MeasurementObjectType type_;
        DataDistribution* dataDistributionInterface_;

    private:
        std::mutex processingMtx_;
        bool isProcessing_;
        bool showGui_;
        int sleepDuration_;
        double amplitude_;
        double phase_;
        double frequency_;
        std::unique_ptr<std::thread> processingThread_;
        void generate();
        
    public:
        WaveGenerator(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
        virtual ~WaveGenerator();
        virtual void* getInterface(const std::string& interfaceName) override;
        virtual void startProcessing() override;
        virtual void endProcessing() override;
        virtual const uint8_t& getInstanceNumber() override;
        virtual const uint64_t& getHandle() override;
        virtual const MeasurementObjectType& getType() override;
        virtual const std::string& getName() override;
        virtual void initializeObject() override;
        virtual void terminateObject() override;
        void show(ImGuiContext* ctx) override;
    };
}

extern "C" WAVE_DATA_SOURCE_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle)
{
    return new data_source::WaveGenerator(interfaceAccess, instanceNb, name, handle);
}