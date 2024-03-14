#include "pch.h"
#include <core/ConfigurationManager.hpp>
#include <XmlWrapper.hpp>
#include <algorithm>
#include <core/DistributionManager.hpp>
#include <defs/Receiver.hpp>
#include <defs/Transmitter.hpp>
#include <sstream>

namespace core
{
    ConfigurationManager::ConfigurationManager(InterfaceAccess* interfaceAccess, std::shared_ptr<MeasurementObjectFactory> factory):
        interfaceAccess_(interfaceAccess),
        factory_(factory)
    {
        logger_ = static_cast<LoggingInterface*>(interfaceAccess_->getInterface("LoggingInterface"));
        logger_->subscribe("ConfigurationManager", CONFIGURATION_MGR_HANDLE);
    }
    const MeasurementObjectList& ConfigurationManager::loadConfiguration(std::filesystem::path path)
    {
        if(std::filesystem::exists(path))
            return measurementObjectList_;

        measurementObjectList_.clear();

        //! @todo parse the XML file and repoputlate the object list use the xml wrapper
        //utility::parser::XmlWrapper xml;

        return measurementObjectList_;
    }

    void* ConfigurationManager::getInterface(const std::string& interfaceName)
    {
        if(interfaceName == "ConfigurationParser")
        {
            return dynamic_cast<ConfigurationParser*>(this);
        }
        if(interfaceName == "ConfigurationFactory")
        {
            return std::dynamic_pointer_cast<ConfigurationFactory>(factory_).get();
        }
        if(interfaceAccess_)
        {
            return interfaceAccess_->getInterface(interfaceName);
        }
        return nullptr;
    }

    bool ConfigurationManager::createMeasurementObject(const std::string& name, uint8_t instanceNb, const std::string& alias)
    {
        std::string msg = "Started creating object: " + name + " with instance number: " + std::to_string((int) instanceNb);
        logger_->log(msg.c_str(), CONFIGURATION_MGR_HANDLE);
        size_t sizeBeforeCreate = measurementObjectList_.size();
        MeasurementObjectPtr mo = factory_->createMeasurementObject(name, instanceNb, alias);
        if (!mo)
        {
            return false;
        }

        if(std::any_of(measurementObjectList_.begin(), measurementObjectList_.end(), [&](const MeasurementObjectPtr obj)
            { 
                return obj->getHandle() == mo->getHandle();
            }))
        {
            auto objectControlIfc = dynamic_cast<ObjectControl*>(mo);

            if (objectControlIfc)
            {
                objectControlIfc->terminateObject();
            }
            logger_->log("Failed to create duplicate measurement object", CONFIGURATION_MGR_HANDLE, severity::error);
            return false;
        }

        if(mo->getType() == MeasurementObjectType::data_receiver)
        {
            if(std::any_of(measurementObjectList_.begin(), measurementObjectList_.end(), [&](const MeasurementObjectPtr obj)
            { 
                return obj->getName() == mo->getName();
            }))
            {
                logger_->log("There is already a identical processor present in the configuration manager", CONFIGURATION_MGR_HANDLE, severity::error);
                
                return false;
            }
            auto ifc = static_cast<DistributionManagerPrivate*>(interfaceAccess_->getInterface("DistributionManagerPrivate"));
            ifc->addReceiver(dynamic_cast<DataReceiverObjectPtr>(mo));
        }

        measurementObjectList_.push_back(mo);

        auto objectControlIfc = dynamic_cast<ObjectControl*>(mo);

        if (objectControlIfc)
        {
            objectControlIfc->initializeObject();
        }

        std::string msg2 = "Finished creating object: " + name + " with instance number: " + std::to_string((int) instanceNb);
        logger_->log(msg2.c_str(), CONFIGURATION_MGR_HANDLE);
        return sizeBeforeCreate < measurementObjectList_.size();
    }
    
    bool ConfigurationManager::removeMeasurementObject(const std::string& name)
    {
        auto it = std::remove_if(measurementObjectList_.begin(), measurementObjectList_.end(), [&]
        (auto obj)
        {
            return obj->getName() == name;
        });

        if(it == measurementObjectList_.end())
        {
            return false;
        }

        auto objectControlIfc = dynamic_cast<ObjectControl*>(*it);

        if (objectControlIfc)
        {
            objectControlIfc->terminateObject();
        }

        measurementObjectList_.erase(it, measurementObjectList_.end());

        return true;
    }

    const MeasurementObjectList& ConfigurationManager::getMOsAddedInConfig()
    {
        return measurementObjectList_;
    }

    bool ConfigurationManager::createMeasurementObject(MeasurementObjectPtr object)
    {
        if(std::any_of(measurementObjectList_.begin(), measurementObjectList_.end(), [&](const MeasurementObjectPtr obj)
            { 
                return obj->getHandle() == object->getHandle();
            }))
        {
            logger_->log("Failed to create duplicate measurement object", CONFIGURATION_MGR_HANDLE, severity::error);
            return false;
        }

        measurementObjectList_.push_back(object);
        return true;
    }

    void ConfigurationManager::terminate()
    {
        clearMeasurementObjectList();
        factory_.reset();
    }

    ConfigurationManager::~ConfigurationManager()
    {
        clearMeasurementObjectList();
    }

    void ConfigurationManager::clearMeasurementObjectList()
    {
        for (MeasurementObjectPtr object : measurementObjectList_)
        {
            try
            {
                auto objectControlIfc = dynamic_cast<ObjectControl*>(object);

                if (objectControlIfc)
                {
                    objectControlIfc->terminateObject();
                }
            }
            catch (const std::exception& ex)
            {
                logger_->log(ex.what(), CONFIGURATION_MGR_HANDLE, severity::critical);
            }
        }
        measurementObjectList_.clear();
    }

    void ConfigurationManager::show(ImGuiContext* ctx)
    {
        ImGui::SetCurrentContext(ctx);
        for (MeasurementObjectPtr object : measurementObjectList_)
        {
            auto objectControlIfc = dynamic_cast<GuiControlIfc*>(object);

            if (objectControlIfc)
            {
                objectControlIfc->show(ctx);
            }
        }
    }

    bool ConfigurationManager::showModal(ImGuiContext* ctx)
    {
        ImGui::SetCurrentContext(ctx);
        bool modalResult = true;
        ImGui::OpenPopup("Configuration manager");
        if (ImGui::BeginPopupModal("Configuration manager", &modalResult, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if(ImGui::TreeNodeEx("Measurement objects available in configuration manager", ImGuiTreeNodeFlags_Framed))
            {
                for (auto& entryMO : factory_->getFactoryMap())
                {
                    if (ImGui::Button(entryMO.first.c_str(), ImVec2(400, 0)))
                    {
                        
                        ImGui::OpenPopup(("Create measurement object" + entryMO.first).c_str());
                    }

                    if (ImGui::BeginPopupModal(("Create measurement object" + entryMO.first).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::InputText("Measurement object name", &localMoName);
                        ImGui::InputInt("Measurement object instance number", &localMoInstanceNb);
                        
                        if (ImGui::Button("OK"))
                        {
                            createMeasurementObject(entryMO.first, static_cast<uint8_t>(localMoInstanceNb), localMoName);
                            
                            ImGui::CloseCurrentPopup();
                            
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::TreePop();
            }

            if(ImGui::TreeNodeEx("Existing measurement objects", ImGuiTreeNodeFlags_Framed))
            {
                for (MeasurementObjectPtr object : measurementObjectList_)
                {
                    if(ImGui::TreeNodeEx(object->getName().c_str(), ImGuiTreeNodeFlags_Framed))
                    {
                        auto extObject = dynamic_cast<ExtendedMeasurementObject*>(object);

                        if (extObject)
                        {
                            ImGui::BeginTable("Property table", 2, ImGuiTableFlags_Borders);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Entry name");
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("Entry value");
                            for (auto& entry : extObject->getPropertyTable())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("%s", entry.first.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%s", entry.second.c_str());
                            }
                            ImGui::EndTable();
                        }

                        if(object->getType() != MeasurementObjectType::system)
                        {
                            if(ImGui::Button("Remove", ImVec2(400, 0)))
                            {
                                if(removeMeasurementObject(object->getName()))
                                {
                                    ImGui::TreePop();
                                    ImGui::TreePop();
                                    ImGui::EndPopup();
                                    return true;
                                }
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }

            ImGui::EndPopup();
        }
        return modalResult;
    }
}