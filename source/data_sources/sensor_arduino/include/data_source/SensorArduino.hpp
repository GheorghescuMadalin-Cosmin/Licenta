#pragma once
#include <visibility.h>
#include <defs/MdsInterface.hpp>
#include <defs/Distribution.hpp>
#include <memory>
#include <defs/Distribution.hpp>
#include <defs/Transmitter.hpp>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <string>
#include <iostream>
#include <defs/Log.hpp>
namespace data_source
{
    class SENSOR_DATA_SOURCE_API SerialPort: public InterfaceAccess,
        public MeasurementObject,
        public DataSenderObject,
        public ObjectControl,
        public GuiControlIfc{
            
        public:
            HANDLE hSerial;
            DataDistribution* dataDistributionInterface_;
            std::unique_ptr<std::thread> processingThread_;
            void generate();
            bool isProcessing_;
            bool showGui_;
            uint8_t instanceNumber_;
            uint64_t handle_;
            std::mutex processingMtx_;
            std::string name_;
            LoggingInterface *logger_;
            MeasurementObjectType type_;
            InterfaceAccess* interfaceAccess_;
            SerialPort(InterfaceAccess* interfaceAccess,const uint8_t instanceNb,const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
            ~SerialPort();
            int readData();
            void show(ImGuiContext* ctx) override;
            void initialize();
            virtual void* getInterface(const std::string& interfaceName) override;
            virtual void startProcessing() override;
            virtual void endProcessing() override;
            virtual const uint8_t& getInstanceNumber() override;
            virtual const uint64_t& getHandle() override;
            virtual const MeasurementObjectType& getType() override;
            virtual const std::string& getName() override;
            virtual void initializeObject() override;
            virtual void terminateObject() override;

    };
}

extern "C" SENSOR_DATA_SOURCE_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle)
{
    return new data_source::SerialPort(interfaceAccess, instanceNb, name, handle);
}