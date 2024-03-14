#include "pch.h"
#include <WriteFile.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>
#include <implot.h>

visualizer::WriteFile::WriteFile(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
    interfaceAccess_(interfaceAccess),
    instanceNb_(nb),
    handle_(handle(nb, MeasurementObjectType::data_receiver)),
    name_(name + " # " + std::to_string(handle_)),
    type_(MeasurementObjectType::data_receiver),
    showGui_(true)
{
    ImPlot::CreateContext();
    logger_ = static_cast<LoggingInterface*>(interfaceAccess_->getInterface("LoggingInterface"));
    logger_->subscribe(name_.c_str(), handle_);

    arduinoFile_.open("arduino_package.bin", std::ios::app | std::ios::binary);
    if (!arduinoFile_.is_open()) {
        logger_->log("Error: Unable to open arduino_package.bin for writing!", handle_, severity::error);
    }

    webcamFile_.open("webcam_package.bin", std::ios::app | std::ios::binary);
    if (!webcamFile_.is_open()) {
        logger_->log("Error: Unable to open webcam_package.bin for writing!", handle_, severity::error);
    }
}
visualizer::WriteFile::~WriteFile()
{
    interfaceAccess_ = nullptr;

    if (arduinoFile_.is_open()) {
        arduinoFile_.close();
    }

    if (webcamFile_.is_open()) {
        webcamFile_.close();
    }
}


const uint8_t& visualizer::WriteFile::getInstanceNumber()
{
    return instanceNb_;
}
const uint64_t& visualizer::WriteFile::getHandle()
{
    return handle_;
}
const MeasurementObjectType& visualizer::WriteFile::getType()
{
    return type_;
}
const std::string& visualizer::WriteFile::getName()
{
    return name_;
}

bool visualizer::WriteFile::validatePackage(DataPackageCPtr pkg)
{
    if (pkg->type == PackageType::arduino)
    {
        writeArduinoPackage(pkg);
    }
    else if (pkg->type == PackageType::webcam)
    {
        writeWebcamPackage(pkg);
    }
    else
    {
        return false;
    }

    return true;
}

void visualizer::WriteFile::writeArduinoPackage(DataPackageCPtr pkg)
{
   if (!arduinoFile_.is_open()) {
        logger_->log("Error: Arduino file is not open for writing!", handle_, severity::error);
        return;
    }

    arduinoFile_.write(reinterpret_cast<const char*>(&pkg->timestamp), sizeof(uint64_t));
    arduinoFile_.write(reinterpret_cast<const char*>(&pkg->sourceHandle), sizeof(uint64_t));
    arduinoFile_.write(reinterpret_cast<const char*>(&pkg->size), sizeof(size_t));
    arduinoFile_.write(reinterpret_cast<const char*>(&pkg->type), sizeof(PackageType));
    arduinoFile_.write(reinterpret_cast<const char*>(pkg->payload), pkg->size);
}

void visualizer::WriteFile::writeWebcamPackage(DataPackageCPtr pkg)
{   

    if (!webcamFile_.is_open()) {
        logger_->log("Error: Webcam file is not open for writing!", handle_, severity::error);
        return;
    }

    webcamFile_.write(reinterpret_cast<const char*>(&pkg->timestamp), sizeof(uint64_t));
    webcamFile_.write(reinterpret_cast<const char*>(&pkg->sourceHandle), sizeof(uint64_t));
    webcamFile_.write(reinterpret_cast<const char*>(&pkg->size), sizeof(size_t));
    webcamFile_.write(reinterpret_cast<const char*>(&pkg->type), sizeof(PackageType));
    webcamFile_.write(reinterpret_cast<const char*>(pkg->payload), pkg->size);
}

void* visualizer::WriteFile::getInterface(const std::string& interfaceName)
{
    if(interfaceAccess_)
    {
        return interfaceAccess_->getInterface(interfaceName);
    }
    return nullptr;
}

void visualizer::WriteFile::initializeObject()
{
   //virtual method
}

void visualizer::WriteFile::terminateObject()
{
    delete this;
}

void visualizer::WriteFile::show(ImGuiContext* ctx)
{
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