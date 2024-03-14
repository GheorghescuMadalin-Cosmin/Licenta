#pragma once
#include <cstdint>
#include <defs/DataPackage.hpp>

/*!
*   @brief Interface used by any processor in order to receive packages from the distribution manager
*/
struct DataReceiverObject
{
    /*!
    *   @brief Process data package.
    *   @note The package cannot be altered, but a new package can be created to be delivered to the subscribers
    *   @param package Pointer to a const data package that will be analyzed and proccessed.
    *   @return Return true if all the processors validate the package, false if any proccesor cannot validate it.
    */
    virtual bool validatePackage(DataPackageCPtr package) = 0;
};

//! Data processor pointer
using DataReceiverObjectPtr = DataReceiverObject*;

/*!
*   @brief Interface used to notify all the subject registered using registerToReceiverSink method to a processor
*/
struct NotifySubjects
{
    /*!
    *   @brief Method use to notify a subscriber when a data package is received.
    *   @param pkg data package that will be transmitted to the subject
    *   @return Return true if the subject was notified.
    */
    virtual bool notify(DataPackageCPtr pkg) = 0;
};

/*!
*   @brief Interface used to register a subject to a processor
*/
struct ReceiverSinkManager
{
    /*!
    *   @brief Method use to register a subject to a processor.
    *   @param subject subject that will be stored in the notification sink of the processor
    *   @return Return true if the subject was introduced to the processor sink
    */
    virtual bool registerToReceiverSink(NotifySubjects* subject) = 0;

    /*!
    *   @brief Method use to unregister a subject to a processor.
    *   @param subject subject that will be cleared from the notification sink of the processor
    *   @return Return true if the subject was cleared from the processor sink
    */
    virtual bool unregisterToReceiverSink(NotifySubjects* subject) = 0;
};