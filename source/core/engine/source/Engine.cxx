#include <core/Engine.hpp>
#include "pch.h"
#include <defs/GuiDefs.hpp>
#include <core/MeasurementObjectFactory.hpp>
#include <core/DistributionManager.hpp>
#include <Logger.hpp>
#include "Watchdog.hpp"
#include "MemoryVisualization.hpp"
#include "CPUMonitor.hpp"
#include "SignalExtraction.hpp"

namespace core
{
    Engine::Engine(EngineInitFlag flag):
        configMgr_(nullptr),
        dataDistributionPtr_(nullptr),
        logger_(nullptr),
        watchdog_(nullptr),
        self_(nullptr),
        interfaceHelperPtr_(nullptr),
        showLogger_(true),
        showDistrMgr_(false),
        showAbout_(false),
        showConfigurationManager_(false),
        memoryMonitor_(nullptr),
        initTerminationPhaseFlag_(false),
        cpuMonitor_(nullptr),
        signalExtractionIfc_(nullptr)
    {
        bool silentLog = false;

        switch (flag)
        {
        case EngineInitFlag::normal:
            break;
        case EngineInitFlag::silent:
        {
            silentLog = true;
        }
        break;
        case EngineInitFlag::no_metrics:
        {
            silentLog = true;
        }
        break;
        case EngineInitFlag::performance:
        {
            silentLog = true;
        }
        break;
        default:
            break;
        }

        interfaceHelperPtr_ = new core::utility::InterfaceUtilityHelper(this);
        logger_ = new Logger(this, silentLog);
        dataDistributionPtr_ = new DistributionManager(this);
        watchdog_ = new metrics::Watchdog(logger_);
        memoryMonitor_ = new MemoryVisualization();
        cpuMonitor_ = new CPUMonitor();
        signalExtractionIfc_ = new core::signal::SignalExtraction(this);
        
    }
    Engine::~Engine()
    {
        terminate();
    }


    void* Engine::getInterface(const std::string& ifcName)
    {
        if(ifcName.find("LoggingInterface") != std::string::npos)
        {
            return dynamic_cast<LoggingInterface*>(logger_);
        }
        if (ifcName.find("DataDistributionStatistics") != std::string::npos)
        {
            return dataDistributionPtr_->getDistributionInterface();
        }
        if (ifcName.find("DistributionManagerPrivate") != std::string::npos)
        {
            return dynamic_cast<DistributionManagerPrivate*>(dataDistributionPtr_);
        }
        if(ifcName.find("DataDistribution") != std::string::npos)
        {
            return dynamic_cast<DataDistribution*>(dataDistributionPtr_);
        }
        if(ifcName.find("ConfigurationParser") != std::string::npos)
        {
            return dynamic_cast<ConfigurationParser*>(configMgr_);
        }
        if(ifcName.find("ConfigurationFactory") != std::string::npos)
        {
            return configMgr_->getInterface(ifcName);
        }
        if(ifcName.find("SignalExtractionInterface") != std::string::npos)
        {
            return signalExtractionIfc_;
        }

        return nullptr;
    }

    void Engine::initialize()
    {
        initTerminationPhaseFlag_ = false;
        logger_->log("Started initialization", ENGINE_HANDLE, severity::debug);
        std::shared_ptr<MeasurementObjectFactory> factory = std::make_shared<MeasurementObjectFactory>(this);
        configMgr_ = new ConfigurationManager(this, factory);
        self_ = new EngineObject();

        if(!configMgr_->createMeasurementObject(self_))
        {
            logger_->log("Failed to introduce Engine Object in the configuration manager", ENGINE_HANDLE, severity::critical);
        }

        configMgr_->createMeasurementObject(watchdog_);
        logger_->log("Initialization finished", ENGINE_HANDLE, severity::debug);
    }
    
    void Engine::terminate()
    {
        if(initTerminationPhaseFlag_) return;

        initTerminationPhaseFlag_ = true;
        logger_->log("Started engine termination");
        
        if (dataDistributionPtr_)
        {
            logger_->log("Stopping data distribution", ENGINE_HANDLE, severity::information);
            dataDistributionPtr_->stopDistribution();
        }

        if (configMgr_)
        {
            logger_->log("Destroying configuration manager", ENGINE_HANDLE, severity::information);
            configMgr_->terminate();
            delete configMgr_;
            configMgr_ = nullptr;
        }

        if (dataDistributionPtr_)
        {
            logger_->log("Destroying data distribution manager", ENGINE_HANDLE, severity::information);
            delete dataDistributionPtr_;
            dataDistributionPtr_ = nullptr;
        }

        if(watchdog_)
        {
            logger_->log("Destroying the watchdog");
            delete watchdog_;
            watchdog_ = nullptr;
        }

        if(memoryMonitor_)
        {
            logger_->log("Destroying the memory monitor");
            delete reinterpret_cast<MemoryVisualization*>(memoryMonitor_);
            memoryMonitor_ = nullptr;
        }

        if(self_)
        {
            logger_->log("Destroying the engine object");
            delete self_;
            self_ = nullptr;
        }

        if(interfaceHelperPtr_)
        {
            logger_->log("Destroying the interface helper");
            delete interfaceHelperPtr_;
            interfaceHelperPtr_ = nullptr;
        }

        logger_->log("Finished engine termination");

        if(logger_)
        {
            delete logger_;
            logger_ = nullptr;
        }
        
    }

    bool Engine::isWatchDogActive()
    {
        return watchdog_ != nullptr;
    }

    bool Engine::isLoggerActive()
    {
        return logger_ != nullptr;
    }
    bool Engine::isPerformingDataAquisition()
    {
        return dataDistributionPtr_->isDistributing();
    }

    core::utility::InterfaceUtilityHelper* Engine::getInterfaceHelper()
    {
        return interfaceHelperPtr_;
    }

    void Engine::show(ImGuiContext* ctx)
    {
        if(initTerminationPhaseFlag_) return;

        INIT_CONTEXT(ctx);

        BEGIN_MAIN_MENU_BAR
            BEGIN_MENU("File")
                ADD_MENU_ITEM("Open configuration manager", "Ctrl+o", showConfigurationManager_);
            END_MENU

            BEGIN_MENU("Show")
                ADD_MENU_ITEM(showDistrMgr_ ? "Hide distribution manager" : "Show distribution manager", "Ctrl+d", showDistrMgr_);
                ADD_MENU_ITEM(showLogger_ ? "Hide log monitor" : "Show log monitor", "Ctrl+l()", showLogger_); 
            END_MENU
            
            BEGIN_MENU("About")
                ADD_MENU_ITEM("About engine", "Ctrl+a", showAbout_);
            END_MENU
        END_MAIN_MENU_BAR

        DISPLAY_MODAL_WINDOW(showConfigurationManager_, configMgr_, ctx)
        DISPLAY_ABOUT_MENU(showAbout_, "About engine")
        DISPLAY_WINDOW(showDistrMgr_, dataDistributionPtr_, ctx)
        DISPLAY_WINDOW(showLogger_, logger_, ctx)
        DISPLAY_WINDOW(true, memoryMonitor_, ctx)
        DISPLAY_WINDOW(true, configMgr_, ctx)
        DISPLAY_WINDOW(true, cpuMonitor_, ctx)
    }
}