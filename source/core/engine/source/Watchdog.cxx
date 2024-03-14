#include "pch.h"
#include <chrono>
#include <cstdint>
#include "Watchdog.hpp"
#include <defs/MdsInterface.hpp>
#include <inttypes.h>
#include <defs/GuiDefs.hpp>
#include <imgui_internal.h>

namespace core
{
    namespace metrics
    {
        using namespace std::chrono_literals;
        void Watchdog::watch()
        {
            while(alive_)
            {
                {
                    std::lock_guard<std::mutex> lock(timestampGuard_);
                    uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    deltaTimestamp_ = timestamp - lastTimestamp_;

                    if(lastTimestamp_ != 0 && deltaTimestamp_ > (uint64_t)maxDuration_)
                        logger_->log(("Timegap of:" + std::to_string(deltaTimestamp_) + " was detected. Possible hardware deadlock.").c_str(), 3, severity::warning);

                    lastTimestamp_ = timestamp;
                }

                std::this_thread::sleep_for(1ms);
            }
        }

        Watchdog::Watchdog(LoggingInterface* logger):
            logger_(logger),
            alive_(true),
            lastTimestamp_(0),
            deltaTimestamp_(0),
            instanceNumber_(0),
            handle_(WATCHDOG_HANDLE),
            type_(MeasurementObjectType::system),
            name_("Watchdog"),
            showWatchdog_(true),
            maxDuration_(0xffff)
        {
            watchThread_ = std::make_unique<std::thread>(&Watchdog::watch, this);
            logger_->subscribe("Watchdog", WATCHDOG_HANDLE);
            logger_->log("Watchdog started", WATCHDOG_HANDLE, severity::information);
        }
        Watchdog::~Watchdog()
        {
            std::lock_guard<std::mutex> lock(timestampGuard_);
            logger_->log("Watchdog stopped", WATCHDOG_HANDLE, severity::information);
            logger_->unsubscribe(WATCHDOG_HANDLE);
            alive_ = false;
            watchThread_->join();
            watchThread_.reset();
        }

        void Watchdog::show(ImGuiContext* ctx)
        {
            INIT_CONTEXT(ctx)
            BEGIN_MAIN_MENU_BAR
                BEGIN_MENU("Show")
                    ADD_MENU_ITEM(!showWatchdog_ ? "Show watchdog" : "Hide watchdog", "Ctrl+w", showWatchdog_)
                END_MENU
            END_MAIN_MENU_BAR

            if (showWatchdog_)
            {
                ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
                if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
                {
                    if(ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_Framed))
                    {
                        ImGui::Text("Last timestamp: %" PRIu64, lastTimestamp_);
                        ImGui::Text("Delta timestamp (microseconds): %" PRIu64, deltaTimestamp_);
                        ImGui::SliderInt("Time",&maxDuration_, 0, 0xffff);
                        ImGui::TreePop();
                    }
                    
                    ImGui::End();
                }
            }

            
        }

        const uint8_t& Watchdog::getInstanceNumber()
        {
            return instanceNumber_;
        }

        const uint64_t& Watchdog::getHandle()
        {
            return handle_;
        }

        const MeasurementObjectType& Watchdog::getType()
        {
            return type_;
        }

        const std::string& Watchdog::getName()
        {
            return name_;
        }

        bool Watchdog::hasPropertyTable()
        {
            return true;
        }

        bool Watchdog::insertEntry(const PropertyPair& entryPair)
        {
            return propertyTable_.insert(entryPair).second;
        }

        bool Watchdog::removeProperty(const std::string& propertyName)
        {
            return propertyTable_.erase(propertyName) > 0;
        }

        void Watchdog::clearPropertyTable()
        {
            propertyTable_.clear();
        }
        const PropertyTable& Watchdog::getPropertyTable()
        {
            return propertyTable_;
        }
        const std::string& Watchdog::getPropertyEntryValue(const std::string& entry)
        {
            return propertyTable_.at(entry);
        }
    }
}