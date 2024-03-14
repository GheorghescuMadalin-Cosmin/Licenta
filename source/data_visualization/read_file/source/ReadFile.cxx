#include "pch.h"
#include <ReadFile.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>
#include <implot.h>

visualizer::ReadFile::ReadFile(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
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
}
visualizer::ReadFile::~ReadFile()
{
    interfaceAccess_ = nullptr;
    dataDistributionInterface_ = nullptr;

    if(processingThread_1)
    {
        processingThread_1->join();
        processingThread_1.reset();
    }

    if(processingThread_2)
    {
        processingThread_1->join();
        processingThread_1.reset();
    }
}


const uint8_t& visualizer::ReadFile::getInstanceNumber()
{
    return instanceNb_;
}
const uint64_t& visualizer::ReadFile::getHandle()
{
    return handle_;
}
const MeasurementObjectType& visualizer::ReadFile::getType()
{
    return type_;
}
const std::string& visualizer::ReadFile::getName()
{
    return name_;
}

bool visualizer::ReadFile::validatePackage(DataPackageCPtr pkg)
{
    if (pkg->payload == nullptr)
    {
        logger_->log("Error: Package payload is null!", handle_, severity::error);
        return false;
    }

    if (pkg->size == 0)
    {
        logger_->log("Error: Package size is zero!", handle_, severity::error);
        return false;
    }

    if (pkg->type != PackageType::arduino && pkg->type != PackageType::webcam)
    {
        logger_->log("Error: Unknown package type!", handle_, severity::error);
        return false;
    }

    return true;
}

void visualizer::ReadFile::readArduinoPackage()
{  
    std::string fileName="arduino_package.bin";
    std::ifstream file1(fileName, std::ios::in | std::ios::binary);
    if (!file1.is_open())
    {
        logger_->log("Error: Unable to open arduino_package.bin for reading!", handle_, severity::error);
        return;
    }

    while(true)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        file1.read(reinterpret_cast<char*>(&pkg->timestamp), sizeof(uint64_t));
        file1.read(reinterpret_cast<char*>(&pkg->sourceHandle), sizeof(uint64_t));
        file1.read(reinterpret_cast<char*>(&pkg->size), sizeof(size_t));
        file1.read(reinterpret_cast<char*>(&pkg->type), sizeof(PackageType));

        pkg->payload = new uint8_t[pkg->size];
        if (pkg->payload == nullptr)
        {
            logger_->log("Error: Failed to allocate memory for payload!", handle_, severity::error);
            return;
        }

        file1.read(reinterpret_cast<char*>(pkg->payload), pkg->size);

        if (file1.peek() == EOF)
        {
            break;
        }

        dataDistributionInterface_->distributeData(pkg);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
    }

    file1.close();
}  

void visualizer::ReadFile::readWebcamPackage()
{ 
    std::string fileName="webcam_package.bin";
    std::ifstream file2(fileName, std::ios::in | std::ios::binary);
    if (!file2.is_open())
    {
        logger_->log("Error: Unable to open arduino_package.bin for reading!", handle_, severity::error);
        return;
    }

    while(true)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        file2.read(reinterpret_cast<char*>(&pkg->timestamp), sizeof(uint64_t));
        file2.read(reinterpret_cast<char*>(&pkg->sourceHandle), sizeof(uint64_t));
        file2.read(reinterpret_cast<char*>(&pkg->size), sizeof(size_t));
        file2.read(reinterpret_cast<char*>(&pkg->type), sizeof(PackageType));

        pkg->payload = new uint8_t[pkg->size];
        if (pkg->payload == nullptr)
        {
            logger_->log("Error: Failed to allocate memory for payload!", handle_, severity::error);
            return;
        }

        file2.read(reinterpret_cast<char*>(pkg->payload), pkg->size);
        // int* payloadInt = static_cast<int*>(pkg->payload); 
        // int value = *payloadInt;

        if (file2.peek() == EOF)
        {
            break;
        }

        dataDistributionInterface_->distributeData(pkg);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
    }

    file2.close();
}

void* visualizer::ReadFile::getInterface(const std::string& interfaceName)
{
    if(interfaceAccess_)
    {
        return interfaceAccess_->getInterface(interfaceName);
    }
    return nullptr;
}

void visualizer::ReadFile::initializeObject()
{

}

void visualizer::ReadFile::terminateObject()
{
    delete this;
}

void visualizer::ReadFile::show(ImGuiContext* ctx)
{
    ImGui::SetCurrentContext(ctx);
     
    if (ImGui::BeginMainMenuBar()) 
    {
        if (ImGui::BeginMenu("Show")) {
            if (ImGui::MenuItem(("Show " + name_ + " Sensor").c_str(), "Ctrl+d")) { showGui_ = !showGui_; }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (showGui_) 
    {
        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        
        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags)) 
        {
            if (ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_Framed)) 
            {
                
                if (ImGui::Button("Read Arduino Package")) 
                {   
                    if(interfaceAccess_)
                    {
                        std::lock_guard<std::mutex> lock(mtx_);
                        processingThread_1.reset();
                        dataDistributionInterface_ = reinterpret_cast<DataDistribution*>(interfaceAccess_->getInterface("DataDistribution"));
                        processingThread_1= std::make_unique<std::thread>(&visualizer::ReadFile::readArduinoPackage, this);
                    }
                }

                if (ImGui::Button("Read Webcam Package")) 
                {
                    if(interfaceAccess_)
                    {
                        std::lock_guard<std::mutex> lock(mtx_);
                        processingThread_2.reset();
                        dataDistributionInterface_ = reinterpret_cast<DataDistribution*>(interfaceAccess_->getInterface("DataDistribution"));
                        processingThread_2 = std::make_unique<std::thread>(&visualizer::ReadFile::readWebcamPackage, this);
                    }
                }

                ImGui::TreePop();
            }

            ImGui::End();
        }
    }

} 