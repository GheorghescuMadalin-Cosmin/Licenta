#pragma once
#include <defs/Log.hpp>
#include <defs/MdsInterface.hpp>
#include <defs/SignalExtraction.hpp>
#include <core/EngineObject.hpp>
#include <core/DistributionManager.hpp>
#include <core/ConfigurationManager.hpp>
#include <utilis/InterfaceUtilityHelper.hpp>

namespace core
{
    /*!
    *   @brief metrics namespace
    */
    namespace metrics
    {
        class Watchdog;
    }

    class Logger;
}
namespace core
{
    /*!
    *   @brief MDS engine definition. The engine is responsable to initialize objects and to 
    * facilitate interface access between them. It is also responsable for object handling.
    */
    class Engine :
        public InterfaceAccess,
        public EngineInit,
        public GuiControlIfc
    {
        ConfigurationManager* configMgr_; //!< configuration manager pointer
        DistributionManager* dataDistributionPtr_; //!< data distribution manager pointer
        Logger* logger_; //!< logging interface
        metrics::Watchdog* watchdog_; //!< watchdog pointerinterfaceHelperPtr_
        EngineObject* self_; //!< mds engine mirror as measurement object.
        core::utility::InterfaceUtilityHelper* interfaceHelperPtr_; //!< engine interface helper
        SignalExtractionInterface* signalExtractionIfc_;

        bool showLogger_; //!< flag to show logger
        bool showDistrMgr_; //!< flag to show data distribution manager
        bool showConfigMgr_; //!< flag to show configuration manager
        bool showAbout_; //!< flag to show about window
        bool showConfigurationManager_; //!< flag to show configuration manager window
        GuiControlIfc* memoryMonitor_;
        bool initTerminationPhaseFlag_; //!< flag to indicate if the engine is in the initialization or termination phase
        GuiControlIfc* cpuMonitor_;
    public:
        /*!
        *   @brief MDS engine contructor.
        *   @param flag Flag indicating how the engine will start
        */
        Engine(EngineInitFlag flag = EngineInitFlag::normal);

        /*!
        *   @brief MDS engine destructor
        */
        virtual ~Engine();

        /*!
        *   @brief Method that retreive the desired interface as a void pointer.
        *   @param interfaceName Interface name as a string
        *   @return Return a void pointer to the requested interface
        *   @warning Check if the returned pointer is not nullptr.
        */
        virtual void* getInterface(const std::string& interfaceName) override;

        /*!
        *   @brief Method use to initialize the engine component.
        *   @note Engine need to be initialized in order to create the configuration manager and the
        * data distribution manager.
        *   @warning make sure that the engine is terminated before deleting it.
        */
        virtual void initialize() override;

        /*!
        *   @brief Method used to terminate the engine component.
        *   @note Need to be called before the destructor.
        */
        virtual void terminate() override;

        /*!
        *   @brief Method that retreive the watchdog status.
        *   @return Return true if the watchdog is active, false otherwise.
        */
        bool isWatchDogActive();

        /*!
        *   @brief Method that retreive the logging status.
        *   @return Return true if the logging is active, false otherwise.
        */
        bool isLoggerActive();

        /*!
        *   @brief Method that retreive the data aquisition status.
        *   @return Return true if the data aquisition is performed, false otherwise.
        */
        bool isPerformingDataAquisition();

        /*!
        *   @brief Method used to retreive a template interface. A wrapper and upgrade over the getInterface method.
        *   @tparam ifc Interface type.
        *   @return Return a pointer of the requested interface.
        */
        template <typename ifc> ifc* getInterface()
        {
            return interfaceHelperPtr_->getInteface<ifc>();
        }

        /*!
        *   @brief Method that retreive the engine interface helper.
        *   @return Return a pointer to the engine interface helper.
        */
        core::utility::InterfaceUtilityHelper* getInterfaceHelper();

        /*!
        *  @brief Method used to show the gui object.
        */
        void show(ImGuiContext* ctx) override;
    };
}