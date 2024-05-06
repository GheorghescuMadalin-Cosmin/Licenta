#include "pch.h"
#include <WaveVisualizer.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <implot.h>

visualizer::WaveVisualizer::WaveVisualizer(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
    interfaceAccess_(interfaceAccess),
    instanceNb_(nb),
    handle_(handle(nb, MeasurementObjectType::data_receiver)),
    name_(name + " # " + std::to_string(handle_)),
    type_(MeasurementObjectType::data_receiver),
    showGui_(true),
    maxPkgInBuffer_(64),
    maxPayloadSize_(64)
{
    ImPlot::CreateContext();
}
visualizer::WaveVisualizer::~WaveVisualizer()
{
    interfaceAccess_ = nullptr;
    history_.clear();
}


const uint8_t& visualizer::WaveVisualizer::getInstanceNumber()
{
    return instanceNb_;
}
const uint64_t& visualizer::WaveVisualizer::getHandle()
{
    return handle_;
}
const MeasurementObjectType& visualizer::WaveVisualizer::getType()
{
    return type_;
}
const std::string& visualizer::WaveVisualizer::getName()
{
    return name_;
}

bool visualizer::WaveVisualizer::validatePackage(DataPackageCPtr pkg)
{
   std::lock_guard<std::mutex> lock(mtx_);

   if (pkg->type==PackageType::arduino && pkg->payload == nullptr)
   {
        return false;
   }
   
   if(pkg->type==PackageType::arduino)
   {    
        DataPackage* dataPackage = pkg.get();
        int* dataPtr = static_cast<int*>(dataPackage->payload);
        int extractedValue = *dataPtr;
        history_[pkg->sourceHandle].push_back(extractedValue);

        for (auto& [handle, history] : history_)
        {
            if(history.size() > (size_t)maxPkgInBuffer_)
            {
                history.erase(history.begin(), history.begin() +  (history.size() - (size_t)maxPkgInBuffer_));
            }
        }
   }

   return true;
}

void* visualizer::WaveVisualizer::getInterface(const std::string& interfaceName)
{
    if(interfaceAccess_)
    {
        return interfaceAccess_->getInterface(interfaceName);
    }
    return nullptr;
}

void visualizer::WaveVisualizer::initializeObject()
{

}

void visualizer::WaveVisualizer::terminateObject()
{
    ImPlot::DestroyContext();
    delete this;
}

void visualizer::WaveVisualizer::show(ImGuiContext* ctx)
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
            ImGui::InputInt("Max packages", &maxPkgInBuffer_);
            ImGui::InputInt("Max payload", &maxPayloadSize_);
            ImGui::TreePop();
        }
            
        ImGui::End();
    }

    if(showGui_)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        ImGui::Begin(name_.c_str());
        if (ImPlot::BeginPlot(name_.c_str(), "Point number", "Distance")) 
        {
            // Plot the data
            for (auto [handle, history] : history_)
            {
                ImPlot::PlotLine(std::to_string(handle).c_str(), history.data(), (int)history.size(), 3.0f);
            }

            // End the ImPlot plot
            ImPlot::EndPlot();
        }
        ImGui::End();
    }
    
}