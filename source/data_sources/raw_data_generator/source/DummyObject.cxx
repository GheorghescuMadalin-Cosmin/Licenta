#include "pch.h"
#include <transmitters/DummyObject.hpp>
#include <chrono>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>

using namespace std::chrono_literals;

namespace transmitters
{
    Dummy::Dummy(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle) :
        interfaceAccess_(interfaceAccess),
        instanceNumber_(instanceNb),
        handle_(handle(instanceNb, MeasurementObjectType::data_source)),
        name_(name + " # " + std::to_string(handle_)),
        type_(MeasurementObjectType::data_source),
        isProcessing_(true),
        showGui_(true),
        sleepDuration_(1000)
    {
    }

    Dummy::~Dummy()
    {
        endProcessing();
        dataDistributionInterface_ = nullptr;
    }
    void Dummy::doFSMProcessing()
    {
        while(true)
        {
            DataPackagePtr pkg = std::make_shared<DataPackage>(); //create a blank package
            pkg->sourceHandle = this->handle_;
            pkg->size = payload_.size();
            pkg->timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            pkg->payload = new char[payload_.size()];
            std::memcpy(pkg->payload, payload_.c_str(), payload_.size());
            
            {
                std::lock_guard<std::mutex> lock(processingMtx_);
                
                if(!isProcessing_)
                {
                    break;
                }
                dataDistributionInterface_->distributeData(pkg);
                
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration_));
        }
    }

    void* Dummy::getInterface(const std::string&)
    {
        return nullptr;
    }
    void Dummy::startProcessing()
    {
        if(interfaceAccess_)
        {
            std::lock_guard<std::mutex> lock(processingMtx_);
            processingThread_.reset();
            dataDistributionInterface_ = reinterpret_cast<DataDistribution*>(interfaceAccess_->getInterface("DataDistribution"));
            processingThread_ = std::make_unique<std::thread>(&transmitters::Dummy::doFSMProcessing, this);
        }
    }
    void Dummy::endProcessing()
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
    const uint8_t& Dummy::getInstanceNumber()
    {
        return instanceNumber_;
    }
    const uint64_t& Dummy::getHandle()
    {
        return handle_;
    }
    const MeasurementObjectType& Dummy::getType()
    {
        return type_;
    }
    const std::string& Dummy::getName()
    {
        return name_;
    }

    void Dummy::initializeObject()
    {
        startProcessing();
    }
    void Dummy::terminateObject()
    {
        endProcessing();
        delete this;
    }

    void Dummy::show(ImGuiContext* ctx)
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
                    ImGui::InputText("Payload", &payload_);
                    ImGui::Text("Handle %" PRIu64, handle_);
                    ImGui::TreePop();
                }
                    
                ImGui::End();
            }
        }
    }
}