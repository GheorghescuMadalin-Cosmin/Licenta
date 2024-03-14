#include "pch.h"
#include <core/EngineObject.hpp>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>


namespace core
{
    EngineObject::EngineObject():
        instanceNumber_(0),
        handle_(ENGINE_HANDLE),
        name_("Engine"),
        type_(MeasurementObjectType::system),
        showGui_(false)
    {
        propertyTable_.insert(std::make_pair("Name", "Engine"));
        propertyTable_.insert(std::make_pair("Type", "System"));
        propertyTable_.insert(std::make_pair("Handle", std::to_string(ENGINE_HANDLE)));
        propertyTable_.insert(std::make_pair("Instance", std::to_string(instanceNumber_)));
    }

    const uint8_t& EngineObject::getInstanceNumber()
    {
        return instanceNumber_;
    }
    const uint64_t& EngineObject::getHandle()
    {
        return handle_;
    }
    const MeasurementObjectType& EngineObject::getType()
    {
        return type_;
    }
    const std::string& EngineObject::getName()
    {
        return name_;
    }

    EngineObject::~EngineObject()
    {
        
    }

    bool EngineObject::hasPropertyTable()
    {
        return !propertyTable_.empty();
    }
    bool EngineObject::insertEntry(const PropertyPair& entryPair)
    {
        return propertyTable_.insert(entryPair).second;
    }
    bool EngineObject::removeProperty(const std::string& propertyName)
    {
        auto it = propertyTable_.find(propertyName);
        
        if(it == propertyTable_.end())
        {
            return false;
        }

        propertyTable_.erase(it);

        return true;
    }
    
    void EngineObject::clearPropertyTable()
    {
        propertyTable_.clear();
    }
    
    const PropertyTable& EngineObject::getPropertyTable()
    {
        return propertyTable_;
    }

    
    // Get the value of a property entry.
    const std::string& EngineObject::getPropertyEntryValue(const std::string& entry)
    {
        // Return the value of the property entry.
        return propertyTable_[entry];
    }

    void EngineObject::show(ImGuiContext* ctx)
    {
        INIT_CONTEXT(ctx)
        
        BEGIN_MAIN_MENU_BAR
            BEGIN_MENU("Show")
                ADD_MENU_ITEM(showGui_ ? "Hide engine MO" : "Show engine MO", "Ctrl+w", showGui_)
            END_MENU
        END_MAIN_MENU_BAR

        if(showGui_)
        {
            ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
            if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
            {
                if(ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    DISPLAY_MAP_ELEMENTS_STR(propertyTable_)
                    ImGui::TreePop();
                }
                
                ImGui::End();
            }
        }
    }
}