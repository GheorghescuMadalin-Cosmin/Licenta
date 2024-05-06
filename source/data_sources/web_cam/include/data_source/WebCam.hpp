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
#include <opencv2/opencv.hpp>
namespace data_source
{
    class WEBCAM_DATA_SOURCE_API WebCam: public InterfaceAccess,
        public MeasurementObject,
        public DataSenderObject,
        public ObjectControl,
        public GuiControlIfc{
           
        public:
            DataDistribution* dataDistributionInterface_;
            std::unique_ptr<std::thread> processingThread_;
            void generate();
            int sleepDuration_;
            bool isProcessing_;
            bool showGui_;
            cv::VideoCapture cap;
            cv::CascadeClassifier carClassifier;
            cv::CascadeClassifier pedestrianClassifier;
            cv::Mat frame;
            uint8_t instanceNumber_;
            uint64_t handle_;
            std::mutex processingMtx_;
            std::string name_;
            LoggingInterface *logger_;
            MeasurementObjectType type_;
            InterfaceAccess* interfaceAccess_;
            WebCam(InterfaceAccess* interfaceAccess,const uint8_t instanceNb,const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
            ~WebCam();
            cv::Mat readData();
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

extern "C" WEBCAM_DATA_SOURCE_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle)
{
    return new data_source::WebCam(interfaceAccess, instanceNb, name, handle);
}