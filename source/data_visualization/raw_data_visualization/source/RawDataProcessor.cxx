#include "pch.h"
#include <RawDataProcessor.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>

namespace processors
{
    RawDataProcessor::RawDataProcessor(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handleFun):
        interfaceAccess_(interfaceAccess),
        instanceNb_(nb),
        handle_(handleFun(nb, MeasurementObjectType::data_receiver)),
        name_(name + " # " + std::to_string(handle_)),
        type_(MeasurementObjectType::data_receiver),
        showGui_(true),
        maxPkgInBuffer_(1024),
        maxPayloadSize_(64)
    {
        
    }
    RawDataProcessor::~RawDataProcessor()
    {
        interfaceAccess_ = nullptr;
        packagesBuffer_.clear();
    }

    const uint8_t& RawDataProcessor::getInstanceNumber()
    {
        return instanceNb_;
    }

    const uint64_t& RawDataProcessor::getHandle()
    {
        return handle_;
    }

    const MeasurementObjectType& RawDataProcessor::getType()
    {
        return type_;
    }

    const std::string& RawDataProcessor::getName()
    {
        return name_;
    }

    bool RawDataProcessor::validatePackage(DataPackageCPtr pkg)
    {
        std::lock_guard<std::mutex> lock(mtx_);
       if (pkg->payload == nullptr)
        {
            return false;
        }
        
        packagesBuffer_.push_back(pkg);

        if (packagesBuffer_.size() > static_cast<size_t>(maxPkgInBuffer_))
        {
            packagesBuffer_.erase(packagesBuffer_.begin(), packagesBuffer_.begin() + (packagesBuffer_.size() - static_cast<size_t>(maxPkgInBuffer_)));
        }

        return true;
    }

    void* RawDataProcessor::getInterface(const std::string& interfaceName)
    {
        if(interfaceAccess_)
        {
            return interfaceAccess_->getInterface(interfaceName);
        }
        return nullptr;
    }

    void RawDataProcessor::initializeObject()
    {
    }
    void RawDataProcessor::terminateObject()
    {
        delete this;
    }
    
    void RawDataProcessor::show(ImGuiContext* ctx)
    {
        ImGui::SetCurrentContext(ctx);

        if(ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Show"))
            {
                if (ImGui::MenuItem(("Show "+ name_ +" MO").c_str(), "Ctrl+r")) { showGui_ = !showGui_; }
                
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
        {
            if(ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_Framed))
            {   
                ImGui::Text("Processing status: %s", isProcessing_ ? "enabled" : "dissabled");
                ImGui::Text("Handle %" PRIu64, handle_);
                ImGui::TreePop();
            }
                
            ImGui::End();
        }

        if (showGui_) 
        {
            ImGui::Begin(name_.c_str(), &showGui_, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.00f));
            if (!packagesBuffer_.empty()) 
            {
                if (ImGui::BeginTable("Raw Data Visualization", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY)) 
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Timestamp");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("Source");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("Type");
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("Size");
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("Data");
                    for (const auto& pkg : packagesBuffer_)  
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%" PRIu64, pkg->timestamp);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%" PRIu64, pkg->sourceHandle);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%" PRIu8, static_cast<uint8_t>(pkg->type));
                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%" PRIu64, pkg->size);
                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("0x%" PRIx64, pkg->payload);
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::PopStyleColor();
            ImGui::End();
        }   
    }

}
