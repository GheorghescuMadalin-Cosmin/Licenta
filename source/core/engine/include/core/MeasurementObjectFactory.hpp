#pragma once
#include <map>
#include <string>
#include <filesystem>

#include <defs/MdsInterface.hpp>
#include <defs/Log.hpp>
#include <utilis/LibUtility.hpp>
#include <defs/Configuration.hpp>

namespace core
{
    /*!
    *   @brief Measurement object factory definition
    */
    class MeasurementObjectFactory:
        public ConfigurationFactory,
        public GuiControlIfc
    {
        FactoryMap objectsMap_; //!< map containing factory defs and their shared library name
        core::utility::LibUtility utilityLibrary_; //!< utility library instance. Use to achive cross platform compilation
        InterfaceAccess* interfaceAccess_; //!< interface access pointer
        LoggingInterface* logger_;

        /*!
        *   @brief Method use to scan the received path for any shared libraries that can be opened.
        *   @param path Path where all the shared libraries will be loaded.
        */
        void scanForMeasurementObjects(std::filesystem::path path);

        /*!
        *   @brief Method use to retreive the executable path.
        *   @return Return the executable path.
        */
        std::string getExePath();
    public:
        /*!
        *   @brief Measurement object factory constructor
        *   @param interfaceAccess interface access pointer
        */
        explicit MeasurementObjectFactory(InterfaceAccess* interfaceAccess);
        
        /*!
        *   @brief Method used to use the factory method from the map to create measurement object based on the shared
        * library name.
        *   @param name Shared library name.
        *   @param instanceNb Measurement object instance number.
        *   @return Return a shared instance of the measurement object.
        */
        MeasurementObjectPtr createMeasurementObject(const std::string& name, uint8_t instanceNb, const std::string& alias = "");

        /*!
        *   @brief Method use to retreive the factory func size.
        *   @return Return factory func size.
        */
        size_t getFactorySize() override;

        /*!
        *   @brief Method use to retreive the factory map.
        *   @return Return factory map.
        */
        const FactoryMap& getFactoryMap() override;

        /*!
        *  @brief Method used to show the gui object.
        */
        void show(ImGuiContext* ctx) override;
    };
}