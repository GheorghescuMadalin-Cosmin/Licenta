#include "pch.h"
#include <data_source/WaveDataGenerator.hpp>
#include <chrono>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>

data_source::WaveGenerator::WaveGenerator(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
    interfaceAccess_(interfaceAccess),
    instanceNumber_(instanceNb),
    handle_(handle(instanceNb, MeasurementObjectType::data_source)),
    name_(name + " # " + std::to_string(handle_)),
    type_(MeasurementObjectType::data_source),
    isProcessing_(true),
    showGui_(true),
    sleepDuration_(1),
    amplitude_(12.00),
    phase_(0),
    frequency_(1.00),
    samplePoints_(1000)

{
}
data_source::WaveGenerator::~WaveGenerator()
{
    endProcessing();
    dataDistributionInterface_ = nullptr;
}
void* data_source::WaveGenerator::getInterface(const std::string&)
{
    return nullptr;
}

void data_source::WaveGenerator::generate()
{
    while(true)
    {
        SineWave wave = SineWave(amplitude_, phase_, frequency_);
        DataPackagePtr pkg = std::make_shared<DataPackage>(&wave, sizeof(SineWave), true, handle_);
        {
            std::lock_guard<std::mutex> lock(processingMtx_);
            
            if(!isProcessing_)
            {
                break;
            }
            dataDistributionInterface_->distributeData(pkg);
            
        }
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<uint64_t>(1000000 / samplePoints_)));
    }
}

void data_source::WaveGenerator::startProcessing()
{
    if(interfaceAccess_)
    {
        std::lock_guard<std::mutex> lock(processingMtx_);
        processingThread_.reset();
        dataDistributionInterface_ = reinterpret_cast<DataDistribution*>(interfaceAccess_->getInterface("DataDistribution"));
        processingThread_ = std::make_unique<std::thread>(&data_source::WaveGenerator::generate, this);
    }
}
void data_source::WaveGenerator::endProcessing()
{
    {
        std::lock_guard<std::mutex> lock(processingMtx_);
        sleepDuration_ = 0;
        isProcessing_ = false;
    }

    if(processingThread_)
    {
        processingThread_->join();
        processingThread_.reset();
    }
}

const uint8_t& data_source::WaveGenerator::getInstanceNumber()
{
    return instanceNumber_;
}

const uint64_t& data_source::WaveGenerator::getHandle()
{
    return handle_;
}

const MeasurementObjectType& data_source::WaveGenerator::getType()
{
    return type_;
}

const std::string& data_source::WaveGenerator::getName()
{
    return name_;
}

void data_source::WaveGenerator::initializeObject()
{
    startProcessing();
}

void data_source::WaveGenerator::terminateObject()
{
    endProcessing();
    delete this;
}

void data_source::WaveGenerator::show(ImGuiContext* ctx)
{
    ImGui::SetCurrentContext(ctx);
    if(ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Show"))
        {
            if (ImGui::MenuItem(("Show "+ name_ +" MO").c_str(), "Ctrl+w")) { showGui_ = !showGui_; }
            
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    if(showGui_)
    {
        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
        {
            if(ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_Framed))
            {
                ImGui::Text("Processing status: %s", isProcessing_ ? "enabled" : "dissabled");
                ImGui::SliderInt("Sleep duration",&sleepDuration_, 1, 1000);
                ImGui::InputDouble("Amplitude", &amplitude_);
                ImGui::InputDouble("Frequency", &frequency_);
                ImGui::InputDouble("Phase", &phase_);
                ImGui::Text("Handle %" PRIu64, handle_);
                ImGui::SliderInt("Sample points" , &samplePoints_, 1, 2000);
                ImGui::TreePop();
            }
                
            ImGui::End();
        }
    }
}