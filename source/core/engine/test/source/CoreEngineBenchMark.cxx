#include "BenchMarkUtilis.hpp"
#include "CoreTestFramework.hpp"

TEST_F(BenchmarkUtilis, MemoryConsumptionForEngineInitialization)
{
   ASSERT_MEM_CONSUMPTION_ENGINE_INIT();
}

TEST_F(BenchmarkUtilis, MemoryConsumptionForMoCreated)
{
   ASSERT_MEM_CONSUMPTION_ENGINE_INIT();
   ASSERT_MEM_CONSUMPTION_MO_CREATED(RECEIVER_LIB_NAME);
}

TEST_F(BenchmarkUtilis, MemoryConsumptionForMultipleMoCreated)
{
   ASSERT_MEM_CONSUMPTION_ENGINE_INIT();
   for(uint8_t idx = 0; idx < 10; idx++)
   {
      ASSERT_MEM_CONSUMPTION_MO_CREATED(TRANSMITTER_LIB_NAME, idx);
   }
}

/**
 * This code checks the memory consumption after the engine initialization.
 */

TEST_F(BenchmarkUtilis, MemoryConsumptionForDataProcessing)
{
   // Check the memory consumption after the engine initialization.
   ASSERT_MEM_CONSUMPTION_ENGINE_INIT();
   // Create the receiver and transmitter MOs.
   CREATE_MO(RECEIVER_LIB_NAME);
   CREATE_MO(TRANSMITTER_LIB_NAME);
   // Check the memory consumption after the data processing.
   ASSERT_MEM_CONSUMPTION_DATA_PROCESSING();
}

TEST_F(BenchMarkUtilisPerf, DataDistributionPerformanceEmptyPackage)
{
   ASSERT_MEM_CONSUMPTION_ENGINE_INIT();
   SMALL_PACKAGE_DISTRIBUTION();
}

TEST_F(BenchMarkUtilisPerf, DataDistributionPerformance1KBPackage)
{
   ASSERT_MEM_CONSUMPTION_ENGINE_INIT();
   SMALL_PACKAGE_DISTRIBUTION();
}