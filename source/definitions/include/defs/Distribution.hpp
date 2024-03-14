#pragma once
#include <cstdint>
#include <defs/DataPackage.hpp>
/*!
*   @brief Data distribution interface. Let distribute a package to all linked the receivers.
*/
struct DataDistribution
{
    /*!
    *   @brief Distribute data to all the available receivers from the receiver pool.
    *   @param package Const data package that will be delived to all the receivers.
    *   @return true if the distribution succedded, false if any of the receiver fails to interpret the package.
    *   @note Data distribution will be performed sequencially.
    */
    virtual bool distributeData(DataPackageCPtr package) = 0;
};

//! Data distribution pointer
using DataDistributionPtr = std::shared_ptr<DataDistribution>;

/*!
* @brief Inteface used to retreive data distribution statistics
*/
struct DataDistributionStatistics
{
    /*!
    *  @brief Method used to retreive the number of processed packages per second.
    *  @return Return the number of processed packages per second.
    */
    virtual const uint64_t& getNumberOfProcessedPackagesPerSecond() = 0;

    /*!
    * @brief Method used to retreive the maximum number of processed packages per second.
    * @return Return the maximum number of processed packages per second. 
    */
    virtual const uint64_t& getMaximumProcessedPackagesPerSecond() = 0;
};