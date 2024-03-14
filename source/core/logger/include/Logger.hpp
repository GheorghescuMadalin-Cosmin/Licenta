#pragma once
#include <defs/Log.hpp>
#include <defs/MdsInterface.hpp>
#include <LogRegistry.hpp>
#include <mutex>
#include <vector>

/*!
*   @brief Core namespace definition.
*/
namespace core
{
    /*!
    *   @brief Logger class. It is responsible for logging messages to the console and to the file.
    */
    class Logger:
        public InterfaceAccess,
        public LoggingInterface,
        public GuiControlIfc
    {
        InterfaceAccess* interfaceAccess_; //!< Pointer to the interface access object.
        bool ignoreDebugMsg_; //!< Flag to ignore debug messages.
        std::mutex loggingGuard_; //!< Mutex to protect the log buffer.
        std::vector<std::string> logBuffer_; //!< Buffer for the log messages.
        
    public:
        /*!
        *   @brief Constructor.
        *   @param interfaceAccess Pointer to the interface access object.
        *   @param ignoreDebug Flag to ignore debug messages.
        */
        Logger(InterfaceAccess* interfaceAccess, bool ignoreDebug = false);

        virtual ~Logger() = default;

        /*!
        *   @brief Retreive a pointer to the desired interface.
        *   @param interfaceName Name of the interface required.
        *   @return a pointer to the requested interface
        *   @warning check the pointer before using it. If the interface doesn't exist it will return a nullptr.
        */
        void* getInterface(const std::string& interfaceName) override;

        /*!
        *   @brief Save the log under a XML format
        *   @todo unimplemented, all the messages will be piped to the out or err
        */
        void save() override;

        /*!
        *   @brief logging method.
        *   @param message log message
        *   @param handle reporter handle. If the reporter is registered using the subscribe method, then the name will apear,
        * otherwise the handle will appear instead. If no handle is passed then the Engine will be the reporter.
        *   @param severity log message severity. By default the severity is set as debug.
        */
        void log(const char* message,const uint64_t handle  = 0, const severity sev = severity::debug) override;

        /*!
        *   @brief subscribe method. Link a name with a handle.
        *   @note This method should be used before logging any messages.
        *   @param name name of the reporter. By default the name is Engine.
        *   @param handle reporter handle. By default the handle is 0 (corresponding to the engine)
        *   @return Return true if the link between the name and the handle was made possible, false otherwise.
        */
        bool subscribe(const char* name = "Engine", const uint64_t handle = 0) override;

        /*!
        *   @brief unsubscribe method. Unink a name with a handle.
        *   @note This method should be used before logging any messages.
        *   @param name name of the reporter. By default the name is Engine.
        *   @param handle reporter handle. By default the handle is 0 (corresponding to the engine)
        *   @return Return true if the unlink between the name and the handle was made possible, false otherwise.
        */
        bool unsubscribe(const uint64_t handle = 0) override;

        /*!
        *  @brief Method used to show the gui object.
        *  @param ctx ImGui context.
        */
        void show(ImGuiContext* ctx) override;

    private:
        LogRegister logRegister_; //!< Log register object.
        bool showAbout_; //!< Flag to show the about window.
        int maximumLogSize_; //!< Maximum log size.
    };
}