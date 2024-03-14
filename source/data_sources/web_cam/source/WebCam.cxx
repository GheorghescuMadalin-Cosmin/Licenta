#include "pch.h"
#include <data_source/WebCam.hpp>
#include <chrono>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>
#include <data_types/SineWave.h>

data_source::WebCam::WebCam(InterfaceAccess* interfaceAccess,const uint8_t instanceNb,const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle): 
   interfaceAccess_(interfaceAccess),
   handle_(handle(instanceNb, MeasurementObjectType::data_source)),
   name_(name + " # " + std::to_string(handle_))
{
    initialize();
}

data_source::WebCam::~WebCam() {
    
}

void data_source::WebCam::initialize() {
    logger_ = static_cast<LoggingInterface*>(interfaceAccess_->getInterface("LoggingInterface"));
    logger_->subscribe(name_.c_str(), handle_);
    cap.open(0);

    if (!cap.isOpened()) 
    {
      logger_->log("Failed to open the USB camera.", handle_, severity::error);
      return ;
    }
    else 
    {
       logger_->log("USB camera opened successfully.", handle_, severity::information);
    }

    if (!carClassifier.load("C:\\Users\\cosmin\\Desktop\\haarcascade_car.xml")) 
    {
       logger_->log("Error loading pedestrian classifier.", handle_, severity::error);
       return ;
    }
}

cv::Mat data_source::WebCam::readData() {
    cap >> frame;
    int height = frame.rows;
    int width = frame.cols;
    auto type = frame.type();
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> cars;
    carClassifier.detectMultiScale(gray, cars, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    for (const auto& rect : cars) 
    {
      cv::rectangle(frame, rect, cv::Scalar(255, 0, 0), 2);
    }

    if (frame.empty()) 
    {
       logger_->log("Failed to capture frame." , handle_, severity::error);
    }

    return frame;
}
const uint8_t& data_source::WebCam::getInstanceNumber()
{
    return instanceNumber_;
}

const uint64_t& data_source::WebCam::getHandle()
{
    return handle_;
}

const MeasurementObjectType& data_source::WebCam::getType()
{
    return type_;
}

const std::string& data_source::WebCam::getName()
{
    return name_;
}

void data_source::WebCam::initializeObject()
{
    startProcessing();
}

void data_source::WebCam::terminateObject()
{
    endProcessing();
    delete this;
}

void data_source::WebCam::generate()
{
   while (true) 
   {
        cv::Mat frame =readData();
        void* voidPtr = static_cast<void*>(frame.data);
        size_t dataSize = frame.total() * frame.elemSize();
        //DataPackagePtr pkg = std::make_shared<DataPackage>(compressedData.data(), compressedData.size(), true, handle_);  
        DataPackagePtr pkg = std::make_shared<DataPackage>(voidPtr,dataSize,PackageType::webcam,true, handle_);
        {
            std::lock_guard<std::mutex> lock(processingMtx_);
            if (!isProcessing_) {
                break;
            }
            dataDistributionInterface_->distributeData(pkg);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(33)); 
    }
}

void data_source::WebCam::startProcessing()
{
     if(interfaceAccess_)
    {
        std::lock_guard<std::mutex> lock(processingMtx_);
        processingThread_.reset();
        dataDistributionInterface_ = reinterpret_cast<DataDistribution*>(interfaceAccess_->getInterface("DataDistribution"));
        processingThread_ = std::make_unique<std::thread>(&data_source::WebCam::generate, this);
    }
}

void data_source::WebCam::endProcessing()
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

void* data_source::WebCam::getInterface(const std::string&)
{
    return nullptr;
}

void data_source::WebCam::show(ImGuiContext* ctx) {
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