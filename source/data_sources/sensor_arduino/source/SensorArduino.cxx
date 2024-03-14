#include "pch.h"
#include <data_source/SensorArduino.hpp>
#include <chrono>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>


data_source::SerialPort::SerialPort(InterfaceAccess* interfaceAccess,const uint8_t instanceNb,const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle): 
   hSerial(INVALID_HANDLE_VALUE),
   interfaceAccess_(interfaceAccess),
   handle_(handle(instanceNb, MeasurementObjectType::data_source)),
   name_(name + " # " + std::to_string(handle_))
{
    initialize();
}

data_source::SerialPort::~SerialPort() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }
}

void data_source::SerialPort::initialize() {
    logger_ = static_cast<LoggingInterface*>(interfaceAccess_->getInterface("LoggingInterface"));
    logger_->subscribe(name_.c_str(), handle_);
    hSerial = CreateFileW(L"\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        logger_->log("Failed to open serial port.", handle_, severity::error);
        return;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        logger_->log("Failed to get serial port state.", handle_, severity::error);
        CloseHandle(hSerial);
        return;
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Baudrate la 9600 bps
    dcbSerialParams.ByteSize = 8;         // 8 biți de date
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 bit de stop
    dcbSerialParams.Parity = NOPARITY;     // Fără paritate
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        logger_->log("Failed to set serial port state..", handle_, severity::error);
        CloseHandle(hSerial);
        return;
    }
    
}

int data_source::SerialPort::readData() {
    char buffer[256];
    DWORD bytesRead;

    if (!ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL)) 
    {   
        logger_->log("Failed to read from serial port.", handle_, severity::error);
        return -1;
    }

    std::string dataString(buffer, bytesRead);
    int distance = std::stoi(dataString);
    return distance;
    
}
const uint8_t& data_source::SerialPort::getInstanceNumber()
{
    return instanceNumber_;
}

const uint64_t& data_source::SerialPort::getHandle()
{
    return handle_;
}

const MeasurementObjectType& data_source::SerialPort::getType()
{
    return type_;
}

const std::string& data_source::SerialPort::getName()
{
    return name_;
}

void data_source::SerialPort::initializeObject()
{
    startProcessing();
}

void data_source::SerialPort::terminateObject()
{
    endProcessing();
    delete this;
}
void data_source::SerialPort::generate()
{
   while (true) 
   {
        int data =readData();
        int* pointerToData = &data;
        DataPackagePtr pkg = std::make_shared<DataPackage>(pointerToData, sizeof(int), PackageType::arduino,true, handle_);
        {
            std::lock_guard<std::mutex> lock(processingMtx_);
            if (!isProcessing_) {
                break;
            }
            dataDistributionInterface_->distributeData(pkg);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }
}
void data_source::SerialPort::startProcessing()
{
     if(interfaceAccess_)
    {
        std::lock_guard<std::mutex> lock(processingMtx_);
        processingThread_.reset();
        dataDistributionInterface_ = reinterpret_cast<DataDistribution*>(interfaceAccess_->getInterface("DataDistribution"));
        processingThread_ = std::make_unique<std::thread>(&data_source::SerialPort::generate, this);
    }
}
void data_source::SerialPort::endProcessing()
{
    {
        std::lock_guard<std::mutex> lock(processingMtx_);
        isProcessing_ = false;
    }

    if(processingThread_)
    {
        processingThread_->join();
        processingThread_.reset();
    }
}

void* data_source::SerialPort::getInterface(const std::string&)
{
    return nullptr;
}

void data_source::SerialPort::show(ImGuiContext* ctx) {
    ImGui::SetCurrentContext(ctx);

    // Meniul principal
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Show")) {
            if (ImGui::MenuItem(("Show " + name_ + " Sensor").c_str(), "Ctrl+d")) { showGui_ = !showGui_; }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showGui_) {
        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        
        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags)) {
            if (ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_Framed)) {
                
                ImGui::Text("Processing status: %s", isProcessing_ ? "enabled" : "dissabled");
                ImGui::Text("Handle %" PRIu64, handle_);
                ImGui::TreePop();
            }
            ImGui::End();
        }
    }
}