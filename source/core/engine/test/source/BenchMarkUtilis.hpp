#pragma once
#include <statistics/MemoryUsageStatistics.hpp>
#include <core/Engine.hpp>
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <malloc.h>
#include "CoreTestFramework.hpp"

#pragma warning(disable: 4996)

using namespace std::chrono_literals;
class BenchmarkUtilis : public ::testing::Test
{
protected:
    std::unique_ptr<core::Engine> engine_;
    size_t initialMemory_;
    
public:
    BenchmarkUtilis():
        engine_(nullptr),
        initialMemory_(0)
    {
        initialMemory_ = core::statistics::getCurrentRSS();
    }
    ~BenchmarkUtilis()
    {
        size_t memLeaked = core::statistics::getCurrentRSS() - initialMemory_;
        EXPECT_LE(memLeaked, 1024 * 1024); // we expect leaks smaller than 1 MB
    }
    virtual void SetUp() override
    {
        if(engine_)
        {
            engine_->terminate();
            engine_.reset();
        }
        engine_ =  std::make_unique<core::Engine>(EngineInitFlag::performance);
        ASSERT_TRUE(engine_ != nullptr);
    }
    virtual void TearDown() override
    {
        ASSERT_TRUE(engine_ != nullptr);
        engine_->terminate();
        engine_.reset();
    }

    void ASSERT_MEM_CONSUMPTION_ENGINE_INIT()
    {
        size_t memBefore = core::statistics::getCurrentRSS();
        engine_->initialize();
        size_t memoryWasted = core::statistics::getCurrentRSS() > memBefore ? core::statistics::getCurrentRSS() - memBefore : 0;
        EXPECT_LE(memoryWasted, 1024 * 1024); // we expect leaks smaller than 1 MB
    }

    void ASSERT_MEM_CONSUMPTION_MO_CREATED(const std::string& name, uint8_t instancenb = 0)
    {
        ConfigurationParser* conf = static_cast<ConfigurationParser*>(engine_->getInterface("ConfigurationParser"));
        ASSERT_TRUE(conf != nullptr);
        size_t memBefore = core::statistics::getCurrentRSS();
        ASSERT_TRUE(conf->createMeasurementObject(name, instancenb));
        conf->clearMeasurementObjectList();
        size_t memoryWasted = core::statistics::getCurrentRSS() > memBefore ? core::statistics::getCurrentRSS() - memBefore : 0;
        EXPECT_LE(memoryWasted, 1024 * 1024); // we expect leaks smaller than 1 MB
    }

    void ASSERT_MEM_CONSUMPTION_DATA_PROCESSING()
    {
        std::this_thread::sleep_for(60s);
    }

    void CREATE_MO(const std::string& name, uint8_t instancenb = 0)
    {
        ConfigurationParser* conf = static_cast<ConfigurationParser*>(engine_->getInterface("ConfigurationParser"));
        ASSERT_TRUE(conf != nullptr);
        ASSERT_TRUE(conf->createMeasurementObject(name, instancenb));
    }
};

class BenchMarkUtilisPerf : public BenchmarkUtilis
{
public:
    virtual void SetUp() override
    {
        BenchmarkUtilis::SetUp();
       
    }
    virtual void TearDown() override
    {
        BenchmarkUtilis::TearDown();
    }

    void DUMMY_DISTRIBUTION()
    {
        ConfigurationParser* conf = static_cast<ConfigurationParser*>(engine_->getInterface("ConfigurationParser"));
        ASSERT_TRUE(conf != nullptr);
        ASSERT_TRUE(conf->createMeasurementObject(RECEIVER_LIB_NAME, 0));
        DataDistribution* dataDistributionPtr = static_cast<DataDistribution*>(BenchmarkUtilis::engine_->getInterface("DataDistribution"));
        DataPackagePtr pkg = std::make_shared<DataPackage>();
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        dataDistributionPtr->distributeData(pkg);

        uint64_t  deltaTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timestamp;
        EXPECT_LE(deltaTimestamp, 1000000); // we expect leaks smaller than 1 sec
    }

    void SMALL_PACKAGE_DISTRIBUTION()
    {
        ConfigurationParser* conf = static_cast<ConfigurationParser*>(engine_->getInterface("ConfigurationParser"));
        ASSERT_TRUE(conf != nullptr);
        ASSERT_TRUE(conf->createMeasurementObject(RECEIVER_LIB_NAME, 0));
        DataDistribution* dataDistributionPtr = static_cast<DataDistribution*>(BenchmarkUtilis::engine_->getInterface("DataDistribution"));
        DataPackagePtr pkg = std::make_shared<DataPackage>();
        pkg->payload = static_cast<void*>(new uint8_t[1024]);
        pkg->size = 1;
        pkg->sourceHandle = ENGINE_HANDLE;
        pkg->timestamp = 1;
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        dataDistributionPtr->distributeData(pkg);

        uint64_t  deltaTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timestamp;
    }
};