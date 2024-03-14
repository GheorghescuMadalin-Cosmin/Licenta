#pragma once
#include <mutex>
#include <thread>
#include <cstdint>
#include <cassert>
#include <defs/Distribution.hpp>

using namespace std::chrono_literals;

namespace core
{
    namespace statistics
    {
        /*!
        *   @brief Data distribution statistics class.
        */
        class DistributionStatistics:
            public DataDistributionStatistics
        {
            uint64_t processedPackagesPerSecond_; //!< Number of processed packages per second.
            uint64_t maxProcessedPackagesPerSecond_; //!< Maximum number of processed packages per second.
            uint64_t failedPackagesCnt_; //!< Number of failed packages.
            uint64_t processedPackagesCnt_; //!< Number of processed packages.
            bool alive_; //!< Flag used to stop the counting thread.

            std::mutex countingMtx_; //!< Mutex used to protect the counting variables.
            std::mutex updateMtx_; //!< Mutex used to protect the update of the statistics variables.
            std::unique_ptr<std::thread> countingThread_; //!< Thread used to count the number of processed packages per second.

            /*!
            *   @brief Counting thread function.
            */
            void count()
            {
                while(true)
                {
                    {
                        std::lock_guard<std::mutex> lock(countingMtx_);
                        std::lock_guard<std::mutex> lock2(updateMtx_);

                        if(!alive_)
                        {
                            return;
                        }
                        processedPackagesPerSecond_ = processedPackagesCnt_ + failedPackagesCnt_;

#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
                        maxProcessedPackagesPerSecond_ = std::max(maxProcessedPackagesPerSecond_, processedPackagesPerSecond_);
#else
                        maxProcessedPackagesPerSecond_ = maxProcessedPackagesPerSecond_ > processedPackagesPerSecond_ ? maxProcessedPackagesPerSecond_ : processedPackagesPerSecond_;
#endif

                        processedPackagesCnt_ = 0;
                        failedPackagesCnt_ = 0;
                    }

                    std::this_thread::sleep_for(999ms);
                }
            }
        public:
            /*!
            *   @brief Distribution statistics constructor.
            */
            DistributionStatistics():
                processedPackagesPerSecond_(0),
                maxProcessedPackagesPerSecond_(0),
                failedPackagesCnt_(0),
                processedPackagesCnt_(0),
                alive_(true)
            {
                countingThread_ = std::make_unique<std::thread>(&DistributionStatistics::count, this);
            }
            /*!
            *   @brief Distribution statistics destructor.
            */
            virtual ~DistributionStatistics()
            {
                {
                    std::lock_guard<std::mutex> lock(countingMtx_);
                    alive_ = false;
                }

                countingThread_->join();
                countingThread_.reset();
            }

            /*!
            *   @brief Update the statistics.
            *   @param processed Flag used to indicate if the package was processed or not.
            *   @note If the package was not processed, the failedPackagesCnt_ will be incremented.
            *   @note If the package was processed, the processedPackagesCnt_ will be incremented.
            */
            void update(bool processed = true)
            {
                std::lock_guard<std::mutex> lock(countingMtx_);
                processed ? processedPackagesCnt_++ : failedPackagesCnt_++;
            }

            /*!
            *   @brief Method used to retreive the number of processed packages per second.
            *   @return Return the number of processed packages per second.
            *   @note This method is thread safe.
            */
            const uint64_t& getNumberOfProcessedPackagesPerSecond() override
            {
                std::lock_guard<std::mutex> lock2(updateMtx_);
                return processedPackagesPerSecond_;
            }

            /*!
            *   @brief Method used to retreive the maximum number of processed packages per second.
            *   @return Return the maximum number of processed packages per second.
            *   @note This method is thread safe.
            */
            const uint64_t& getMaximumProcessedPackagesPerSecond() override
            {
                std::lock_guard<std::mutex> lock2(updateMtx_);
                return maxProcessedPackagesPerSecond_;
            }
        };
    }
}