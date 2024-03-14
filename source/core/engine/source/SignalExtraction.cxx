#include "pch.h"
#include "SignalExtraction.hpp"
#include <string>
#include <opencv2/opencv.hpp>

core::signal::SignalExtraction::SignalExtraction(InterfaceAccess* interfaceAccess):
    interfaceAccess_(interfaceAccess)
{

}

void* core::signal::SignalExtraction::getInterface(const std::string& interfaceName)
{
    if (interfaceName == "SignalExtractionInterface")
    {
        return dynamic_cast<SignalExtractionInterface*>(this);
    }
    return interfaceAccess_->getInterface(interfaceName);
}

bool core::signal::SignalExtraction::extract(DataPackageCPtr ptr, SignalGroup& outSignalMap)
{
    switch(ptr->type)
    {
        case PackageType::arduino:
                core::signal::SignalExtraction::extractDataPayload(ptr->payload, ptr->sourceHandle, outSignalMap);
                break;
        case PackageType::webcam:
                core::signal::SignalExtraction::extractWebCamData(ptr->payload, ptr->sourceHandle, outSignalMap);
                break;
    }
    return false;
}

bool core::signal::SignalExtraction::extractWebCamData(void* payload, uint64_t handle, SignalGroup& outSignalMap)
{
    SignalMessage msg;
    cv::Mat frame = *reinterpret_cast<cv::Mat*>(payload);
    int height = frame.rows;
    int width = frame.cols;
    int num_channels = frame.channels();
    int data_type = frame.type();
    int total_elements = frame.total();
    size_t total_size = frame.total() * frame.elemSize();
    msg["Height"] = std::to_string(height);
    msg["Width"] = std::to_string(width);
    msg["Nr channels color"] = std::to_string(num_channels);
    msg["Elements type frame"] = std::to_string(data_type);
    msg["Total elements"] = std::to_string(total_elements);
    msg["Total size frame"] = std::to_string(total_size);
   
    outSignalMap[std::to_string(handle)] = msg;
    return true;
}

bool core::signal::SignalExtraction::extractDataPayload(void* pkg, uint64_t handle, SignalGroup& outSignalMap)
{
    SignalMessage msg;
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    const unsigned char* data = static_cast<const unsigned char*>(pkg);
    while (*data != '\0') 
    {
        oss << std::setw(2) << static_cast<unsigned int>(*data);
        ++data;
   }

    msg["DataPayload"] = oss.str();
    outSignalMap[std::to_string(handle)] = msg;

    return true;
}