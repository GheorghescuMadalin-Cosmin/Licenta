#pragma once

/*!
*   @brief Interface used by any data transmitter in order to transmit packages to the distribution manager
*/
struct DataSenderObject
{
    /*!
    *   @brief start the processing threah that will distribute data to the data distribution manager
    */
    virtual void startProcessing() = 0;
    /*!
    *   @brief end the processing threah that will distribute data to the data distribution manager
    */
    virtual void endProcessing() = 0;
};
//! Data transmitter pointer
using DataSenderObjectPtr = std::shared_ptr<DataSenderObject>;