#include "pch.h"
#include <functional>
#include <core/MeasurementObjectFactory.hpp>
#include <utilis/HandleGenerator.hpp>

typedef MeasurementObjectPtr createMO(InterfaceAccess*, const uint8_t, const char*, std::function<uint64_t(uint8_t, MeasurementObjectType)>);

namespace core
{
    MeasurementObjectFactory::MeasurementObjectFactory(InterfaceAccess* interfaceAccess):
        utilityLibrary_(nullptr),
        interfaceAccess_(interfaceAccess),
        logger_(nullptr)
    {
        
        logger_ = static_cast<LoggingInterface*>(interfaceAccess_->getInterface("LoggingInterface"));
        logger_->subscribe("MeasurementObjectFactory", FACTORY_HANDLE);
        utilityLibrary_ = utility::LibUtility(logger_);

        std::filesystem::path curr_path = std::filesystem::path(getExePath());
        scanForMeasurementObjects(curr_path);
    }

    std::string MeasurementObjectFactory::getExePath() 
    {
#ifdef WIN32
        TCHAR buffer[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, buffer, MAX_PATH);
        std::wstring::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
#else
        return std::filesystem::current_path().string();
#endif
    }

    void MeasurementObjectFactory::scanForMeasurementObjects(std::filesystem::path path)
    {
        logger_->log("Started scanning for measurement objects", FACTORY_HANDLE);

        if(!std::filesystem::exists(path))
        {
            logger_->log("Path does not exist", FACTORY_HANDLE, severity::error);
            return;
        }

        for(auto& obj : std::filesystem::recursive_directory_iterator(path))
        {
            auto extension = obj.path().filename().extension().string();

            if(extension != ".so" && extension != ".dll")
            {   
                logger_->log(("Skipping file: " + obj.path().filename().string()).c_str(), FACTORY_HANDLE);
                continue;
            }

            void* func = utilityLibrary_.openLibrary(obj.path().string(), "createMO");

            if(func)
            {
                logger_->log(("Loaded library: " + obj.path().filename().string()).c_str(), FACTORY_HANDLE, severity::information);
                objectsMap_[obj.path().filename().u8string()] = func;
            }
            else
            {
                if(utilityLibrary_.openLibrary(obj.path().string(), "createReader") == nullptr) //skip readers. They are handled by the ReaderFactory (Player)
                {
                    logger_->log(("Failed to load library: " + obj.path().filename().string()).c_str(), FACTORY_HANDLE, severity::warning);
                }
            }
        }
    }

    MeasurementObject* MeasurementObjectFactory::createMeasurementObject(const std::string& name, uint8_t instanceNb, const std::string& alias)
    {
        logger_->log(("Started creating object: " + name + " with instance number: " + std::to_string((int) instanceNb)).c_str(), FACTORY_HANDLE);
        if(name.empty())
        {
            return nullptr;
        }

        auto it = objectsMap_.find(name.c_str());

        if(it == objectsMap_.end())
        {
            return nullptr;
        }

        createMO* mo = (createMO*)it->second;

        if(!mo)
        {
            return nullptr;
        }
        logger_->log(("Finished creating object: " + name + " with instance number: " + std::to_string((int) instanceNb)).c_str(), FACTORY_HANDLE, severity::information);
        return alias.empty() ? mo(interfaceAccess_, instanceNb, it->first.c_str(), std::bind(&core::utils::generateHandle, std::placeholders::_1, std::placeholders::_2)) : mo(interfaceAccess_, instanceNb, alias.c_str(), std::bind(&core::utils::generateHandle, std::placeholders::_1, std::placeholders::_2));
    }

    size_t MeasurementObjectFactory::getFactorySize()
    {
        return objectsMap_.size();
    }

    const FactoryMap& MeasurementObjectFactory::getFactoryMap()
    {
        return objectsMap_;
    }

    void MeasurementObjectFactory::show(ImGuiContext* ctx)
    {
        ImGui::SetCurrentContext(ctx);
    }
}