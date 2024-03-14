#pragma once
#include <string>
#include <cstdint>
#include <filesystem>
/*!
*   @brief Interface used to load a configurarion file.
*/
struct ConfigurationParser
{
    /*!
    *   @brief loads all the objects with their properties into the configuration manager.
    *   @param path path to the configuration file. The full path must be provided.
    *   @return Return a const list of measurement objects.
    *   @note When loading a configuration, engine will be reseted and reinitialized.
    */
    virtual const MeasurementObjectList& loadConfiguration(std::filesystem::path path) = 0;
    virtual bool createMeasurementObject(const std::string& name, uint8_t instanceNb, const std::string& alias = "") = 0;
    /*!
    *   @brief Method used to introduce an already created measurement object into the configuration manager.
    *   @param object Already created measurement object that will be inserted into the configuration manager.
    *   @return Return true if the object was inserted correctly, false otherwise.
    */
    virtual bool createMeasurementObject(MeasurementObjectPtr object) = 0;

    /*!
    *   @brief Method used to remove a measurement object from the configuration manager.
    *   @param name Measurement object name
    *   @return True if the 
    */
    virtual bool removeMeasurementObject(const std::string& name) = 0;

    virtual void clearMeasurementObjectList() = 0;

    /*!
    *   @brief Retreive the active measurement object lists from the configuration manager.
    *   @return Return a const reference of the measurement object list.
    */
    virtual const MeasurementObjectList& getMOsAddedInConfig() = 0;
};

using FactoryMap = std::map<std::string, void*>;

struct ConfigurationFactory
{
    virtual const FactoryMap& getFactoryMap() = 0;

    /*!
    *   @brief Get the factory size. The factory size represents the number of unique measurement
    * objects that can be created. The factory is populated using dlopen methods.
    *   @return Return the factory size.
    *   @note For more information about the factory read MeasurementObjectFactory class definitions.
    */
    virtual size_t getFactorySize() = 0;
};