#pragma once
#include <cstdint>

/*!
*   @brief severity of the log message enum
*/
enum class severity : uint8_t
{
    debug = 0x00, //!< debug message.
    information = 0x01, //!< information message.
    warning = 0x02, //!< warning message.
    error = 0x03, //!< error message.
    critical = 0x04 //!< critical message.
};

/*!
*   @brief Logging interface. Managed by the engine and use to report messages
*/
struct LoggingInterface
{
    /*!
    *   @brief Save the log under a XML format
    *   @todo unimplemented, all the messages will be piped to the out or err
    */
    virtual void save() = 0;
    /*!
    *   @brief logging method.
    *   @param message log message
    *   @param handle reporter handle. If the reporter is registered using the subscribe method, then the name will apear,
    * otherwise the handle will appear instead. If no handle is passed then the Engine will be the reporter.
    *   @param severity log message severity. By default the severity is set as debug.
    */
    virtual void log(const char* message,const uint64_t handle  = 0, const severity sev = severity::debug) = 0;
    /*!
    *   @brief subscribe method. Link a name with a handle.
    *   @note This method should be used before logging any messages.
    *   @param name name of the reporter. By default the name is Engine.
    *   @param handle reporter handle. By default the handle is 0 (corresponding to the engine)
    *   @return Return true if the link between the name and the handle was made possible, false otherwise.
    */
    virtual bool subscribe(const char* name = "Engine", const uint64_t handle = 0) = 0;
    /*!
    *   @brief unsubscribe method. Unink a name with a handle.
    *   @note This method should be used before logging any messages.
    *   @param name name of the reporter. By default the name is Engine.
    *   @param handle reporter handle. By default the handle is 0 (corresponding to the engine)
    *   @return Return true if the unlink between the name and the handle was made possible, false otherwise.
    */
    virtual bool unsubscribe(const uint64_t handle = 0) = 0;
};