#include "pch.h"
#include <SignalMonitor.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>


visualizer::SignalMonitor::SignalMonitor(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
    interfaceAccess_(interfaceAccess),
    instanceNb_(nb),
    handle_(handle(nb, MeasurementObjectType::data_receiver)),
    name_(name + " # " + std::to_string(handle_)),
    type_(MeasurementObjectType::data_receiver),
    showGui_(true)
{
    sigIfc_ = reinterpret_cast<SignalExtractionInterface*>(interfaceAccess_->getInterface("SignalExtractionInterface"));
}
visualizer::SignalMonitor::~SignalMonitor()
{

}

const uint8_t& visualizer::SignalMonitor::getInstanceNumber()
{
    return instanceNb_;
}
const uint64_t& visualizer::SignalMonitor::getHandle()
{
    return handle_;
}

const MeasurementObjectType& visualizer::SignalMonitor::getType()
{
    return type_;
}

const std::string& visualizer::SignalMonitor::getName()
{
    return name_;
}

bool visualizer::SignalMonitor::validatePackage(DataPackageCPtr pkg)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (pkg->payload == nullptr)
    {
        return false;
    }

    return sigIfc_->extract(pkg, signalContent_);
}

void* visualizer::SignalMonitor::getInterface(const std::string& interfaceName)
{
    if(interfaceAccess_)
    {
        return interfaceAccess_->getInterface(interfaceName);
    }
    return nullptr;
}

void visualizer::SignalMonitor::initializeObject()
{

}
void visualizer::SignalMonitor::terminateObject()
{

}

void visualizer::SignalMonitor::show(ImGuiContext* ctx)
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
            
            ImGui::TreePop();
        }
            
        ImGui::End();
    }

    if(showGui_)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if(ImGui::Begin(name_.c_str(), &showGui_))
        {
            for(auto items : signalContent_)
            {
                if(ImGui::TreeNodeEx(items.first.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    ImGui::BeginTable("Property table", 2, ImGuiTableFlags_Borders);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Signal name");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("Signal value");

                    for (auto sigItem : items.second)
                     {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", sigItem.first.c_str());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", sigItem.second.c_str());
                    }
                    ImGui::EndTable();
                    
                    ImGui::TreePop();
                }
                
            }
            ImGui::End();
        }
    }
}