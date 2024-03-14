#include "pch.h"
#include <core/DistributionManager.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>

namespace core
{
    DistributionManager::DistributionManager(InterfaceAccess* ifcAccess)
        : interfaceAccess_(ifcAccess)
        , distributing_(true)
        , statistics_(nullptr)
    {
        logger_ = static_cast<LoggingInterface*>(interfaceAccess_->getInterface("LoggingInterface"));
        logger_->subscribe("DistributionManager", DISTRIBUTION_MGR_HANDLE);

        statistics_ = new statistics::DistributionStatistics();
    }
    DistributionManager::~DistributionManager()
    {
        logger_->log("Destroying distribution manager", DISTRIBUTION_MGR_HANDLE);
        {
            std::lock_guard<std::mutex> lock(distributionLock_);
            distributing_ = false;
            receiversPool_.clear();
        }

        if(statistics_)
        {
            logger_->log("Destroying distribution statistics", DISTRIBUTION_MGR_HANDLE);
            delete statistics_;
        }
    }

    bool DistributionManager::distributeData(DataPackageCPtr package)
    {
        std::lock_guard<std::mutex> lock(distributionLock_);

        if(!distributing_)
        {
            logger_->log("Distribution manager is not distributing", DISTRIBUTION_MGR_HANDLE, severity::warning);
            return false;
        }

        if(receiversPool_.empty())
        {
            logger_->log("No receivers in pool", DISTRIBUTION_MGR_HANDLE, severity::warning);
            return false;
        }

        bool distributionResult = true;
        for(auto receiver : receiversPool_)
        {
            distributionResult &= receiver->validatePackage(package);
        }
        statistics_->update(distributionResult);
        return distributionResult;
    }
    void DistributionManager::addReceiver(DataReceiverObjectPtr object)
    {
        std::lock_guard<std::mutex> lock(distributionLock_);
        receiversPool_.insert(object);
    }

    void* DistributionManager::getInterface(const std::string& interfaceName)
    {
        std::lock_guard<std::mutex> lock(distributionLock_);
        if(interfaceName == "DataDistribution")
            return dynamic_cast<DataDistribution*>(this);
        if(interfaceName == "DistributionManagerPrivate")
            return dynamic_cast<DistributionManagerPrivate*>(this);
        if(interfaceName == "DistributionManagerPrivate")
            return dynamic_cast<DistributionManagerPrivate*>(this);
        if(interfaceAccess_)
            return interfaceAccess_->getInterface(interfaceName);
        return nullptr;
    }

    void DistributionManager::stopDistribution()
    {
        std::lock_guard<std::mutex> lock(distributionLock_);
        logger_->log("Distribution of data was stopped", DISTRIBUTION_MGR_HANDLE);
        distributing_ = false;
        receiversPool_.clear();
    }

    bool DistributionManager::isDistributing()
    {
        std::lock_guard<std::mutex> lock(distributionLock_);
        return distributing_;
    }

    DataDistributionStatistics* DistributionManager::getDistributionInterface()
    {
        return statistics_;
    }

    void DistributionManager::show(ImGuiContext* ctx)
    {
        std::lock_guard<std::mutex> lock(distributionLock_);
        INIT_CONTEXT(ctx)
        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
        {
            if(ImGui::TreeNodeEx("Distribution manager", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::Text("Distribution status: %s", distributing_ ? "enabled" : "disabled");
                ImGui::Text("Receiver pool size %" PRIu64, receiversPool_.size());
                ImGui::Text("Max packages distributed per second: %" PRIu64, statistics_->getMaximumProcessedPackagesPerSecond());
                ImGui::Text("Actual packages distributed per second: %" PRIu64, statistics_->getNumberOfProcessedPackagesPerSecond());
                ImGui::TreePop();
            }
            
            ImGui::End();
        }
    }
}