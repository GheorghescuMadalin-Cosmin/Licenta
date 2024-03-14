#pragma once
#include <mutex>
#include <memory>
#include <thread>
#include <defs/Log.hpp>
#include <defs/MdsInterface.hpp>

namespace core
{
    namespace metrics
    {
        class Watchdog : 
            public GuiControlIfc,
            public ExtendedMeasurementObject
        {
            LoggingInterface* logger_;
            std::unique_ptr<std::thread> watchThread_;
            bool alive_;
            uint64_t lastTimestamp_;
            uint64_t deltaTimestamp_;
            uint8_t instanceNumber_;
            uint64_t handle_;
            MeasurementObjectType type_;
            std::string name_;
            PropertyTable propertyTable_;
            std::mutex timestampGuard_;
            void watch();
            bool showWatchdog_;
            int maxDuration_;
        public:
            explicit Watchdog(LoggingInterface* loger);
            virtual ~Watchdog();
            Watchdog(const Watchdog& lhs) = delete;
            const Watchdog& operator=(const Watchdog& lhs) = delete;

            /*!
            *   @brief Get Watchdog measurement object instance number.
            *   @return engine instance number.
            */
            const uint8_t& getInstanceNumber() override;

            /*!
            *   @brief Get MDS engine measurement object handle.
            */
            const uint64_t& getHandle() override;

            /*!
            *   @brief Get MDS engine measurement object type.
            */
            const MeasurementObjectType& getType() override;

            /*!
            *   @brief Get MDS engine measurement object name.
            */
            const std::string& getName() override;

            /*!
            *   @brief Method to retreive the property table of the measurement object.
            *   @return Return true if the measurement object has a property table, false otherwise.
            */
            bool hasPropertyTable() override;

            /*!
            *  @brief Method used to insert a new entry to the property table.
            *  @param entryPair pair of strings containing the property name and the property value.
            *  @return Return true if the entry was inserted, false otherwise.
            */
            bool insertEntry(const PropertyPair& entryPair) override;

            /*!
            *  @brief Method used to remove an entry from the property table.
            *  @param propertyName name of the property to be removed.
            *  @return Return true if the entry was removed, false otherwise.
            */
            bool removeProperty(const std::string& propertyName) override;

            /*!
            *  @brief Method used to clear the property table.
            */
            void clearPropertyTable() override;

            /*!
            *  @brief Method used to retreive the property table.
            *  @return Return the property table reference.
            */
            const PropertyTable& getPropertyTable() override;

            /*!
            *  @brief Method used to retreive the value of a property.
            *  @param entry name of the property.
            *  @return Return the value of the property.
            */
            const std::string& getPropertyEntryValue(const std::string& entry) override;

            /*!
            *  @brief Method used to show the gui object.
            */
            void show(ImGuiContext* ctx) override;
        };
    }
}