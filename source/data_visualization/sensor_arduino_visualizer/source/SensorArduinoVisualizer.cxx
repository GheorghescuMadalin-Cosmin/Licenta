#include "pch.h"
#include <SensorArduinoVisualizer.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>
#include <implot.h>

visualizer::SensorArduinoVisualizer::SensorArduinoVisualizer(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
    interfaceAccess_(interfaceAccess),
    instanceNb_(nb),
    handle_(handle(nb, MeasurementObjectType::data_receiver)),
    name_(name + " # " + std::to_string(handle_)),
    type_(MeasurementObjectType::data_receiver),
    showGui_(true),
    maxPkgInBuffer_(1024),
    maxPayloadSize_(64)
{
    ImPlot::CreateContext();
}
visualizer::SensorArduinoVisualizer::~SensorArduinoVisualizer()
{
    interfaceAccess_ = nullptr;
    packagesBuffer_.clear();
}


const uint8_t& visualizer::SensorArduinoVisualizer::getInstanceNumber()
{
    return instanceNb_;
}
const uint64_t& visualizer::SensorArduinoVisualizer::getHandle()
{
    return handle_;
}
const MeasurementObjectType& visualizer::SensorArduinoVisualizer::getType()
{
    return type_;
}
const std::string& visualizer::SensorArduinoVisualizer::getName()
{
    return name_;
}

bool visualizer::SensorArduinoVisualizer::validatePackage(DataPackageCPtr pkg)
{
    if (pkg->type == PackageType::arduino)
    {   
        DataPackage* dataPackage = pkg.get();
        int* dataPtr = static_cast<int*>(dataPackage->payload);
        int extractedValue = *dataPtr;
        packagesBuffer_.push_back(pkg);
        if (packagesBuffer_.size() > static_cast<size_t>(maxPkgInBuffer_))
        {
            packagesBuffer_.erase(packagesBuffer_.begin(), packagesBuffer_.begin() + (packagesBuffer_.size() - static_cast<size_t>(maxPkgInBuffer_)));
        }
        
        return true;
    }
    else
    {
        return false;
    }
}
void* visualizer::SensorArduinoVisualizer::getInterface(const std::string& interfaceName)
{
    if(interfaceAccess_)
    {
        return interfaceAccess_->getInterface(interfaceName);
    }
    return nullptr;
}

void visualizer::SensorArduinoVisualizer::initializeObject()
{
    //virtual method
}

void visualizer::SensorArduinoVisualizer::terminateObject()
{
    ImPlot::DestroyContext();
    delete this;
}

void visualizer::SensorArduinoVisualizer::show(ImGuiContext* ctx)
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

  if (showGui_)
  {
    std::lock_guard<std::mutex> lock(mtx_);
    ImGui::Begin(name_.c_str(), &showGui_, ImGuiWindowFlags_AlwaysAutoResize);
    int lastExtractedValue = 0;
    
    for(auto const& pkg : packagesBuffer_)
    {  
        DataPackage* dataPackage = pkg.get();
        int* dataPtr = static_cast<int*>(dataPackage->payload);
        int extractedValue = *dataPtr;
        
        lastExtractedValue = extractedValue;
    }

    if (lastExtractedValue < 10)
    {  
        // Calculează un factor pentru interpolarea între culori
        float pulseFactor = sin(glfwGetTime() * 5.0f) * 0.5f + 0.5f;

        // Interpolarea între roșu și culoarea de fundal originală
        ImVec4 warningColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImVec4 backgroundColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 interpolatedColor = ImVec4(
            backgroundColor.x + (warningColor.x - backgroundColor.x) * pulseFactor,
            backgroundColor.y + (warningColor.y - backgroundColor.y) * pulseFactor,
            backgroundColor.z + (warningColor.z - backgroundColor.z) * pulseFactor,
            1.0f
        );

        // Setează culoarea textului și afișează textul pulsând
        ImGui::PushStyleColor(ImGuiCol_Text, interpolatedColor);
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Distanta obiect: %d cm", lastExtractedValue);
        ImGui::PopStyleColor();
    }
    else
    {   
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Distanta obiect: %d cm", lastExtractedValue);
    }

  }

    ImGui::End();
} 