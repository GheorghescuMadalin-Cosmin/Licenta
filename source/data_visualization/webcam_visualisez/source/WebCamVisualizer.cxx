#include "pch.h"
#include <WebCamVisualizer.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>
#include <implot.h>

visualizer::WebcamVisualizer::WebcamVisualizer(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle):
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
visualizer::WebcamVisualizer::~WebcamVisualizer()
{
    interfaceAccess_ = nullptr;
    packagesBuffer_.clear();
}


const uint8_t& visualizer::WebcamVisualizer::getInstanceNumber()
{
    return instanceNb_;
}
const uint64_t& visualizer::WebcamVisualizer::getHandle()
{
    return handle_;
}
const MeasurementObjectType& visualizer::WebcamVisualizer::getType()
{
    return type_;
}
const std::string& visualizer::WebcamVisualizer::getName()
{
    return name_;
}

bool visualizer::WebcamVisualizer::validatePackage(DataPackageCPtr pkg)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (pkg->type == PackageType::webcam)
    {
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

void* visualizer::WebcamVisualizer::getInterface(const std::string& interfaceName)
{
    if(interfaceAccess_)
    {
        return interfaceAccess_->getInterface(interfaceName);
    }
    return nullptr;
}

void visualizer::WebcamVisualizer::initializeObject()
{

}

void visualizer::WebcamVisualizer::terminateObject()
{
    ImPlot::DestroyContext();
    delete this;
}

void visualizer::WebcamVisualizer::show(ImGuiContext* ctx)
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
        cv::namedWindow("Frame", cv::WINDOW_NORMAL);
      for (auto it = packagesBuffer_.begin(); it != packagesBuffer_.end(); )
     {
        DataPackagePtr pkg = *it;
        DataPackage* dataPackage = pkg.get();
        unsigned char* rawData = static_cast<unsigned char*>(dataPackage->payload);
        size_t dataSize = dataPackage->size; 
        int height = 480;  
        int width = 640;  
        int type = 16;  
        cv::Mat reconstructedFrame(height, width, type, rawData);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, reconstructedFrame.cols, reconstructedFrame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, reconstructedFrame.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        cv::imshow("Frame", reconstructedFrame);

        if (glIsTexture(textureID) == GL_TRUE)
        {
          it = packagesBuffer_.erase(it); 
        }
        else
        {
          ++it;
        }
     }
        ImGui::End();
    }
}