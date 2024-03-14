#pragma once
#include <defs/MdsInterface.hpp>

namespace core
{
    /*!
    *   @brief MDS engine measurement object
    */
    class EngineObject:
        public ExtendedMeasurementObject,
        public GuiControlIfc
    {
        uint8_t instanceNumber_; //!< engine instance number. Shall be 0
        uint64_t handle_; //!< engine handle. Shall be 0
        std::string name_; //!< engine name
        MeasurementObjectType type_; //!< engine type.
        PropertyTable propertyTable_; //!< engine property table.
        bool showGui_; //!< flag to show the gui object.
    public:
        /*!
        *   @brief MDS engine measurement object constructor.
        */
        EngineObject();

        virtual ~EngineObject();

        /*!
        *   @brief Get MDS engine measurement object instance number.
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

        bool hasPropertyTable() override;
        bool insertEntry(const PropertyPair& entryPair) override;
        bool removeProperty(const std::string& propertyName) override;
        void clearPropertyTable() override;
        const PropertyTable& getPropertyTable() override;
        const std::string& getPropertyEntryValue(const std::string& entry) override;

        /*!
        *  @brief Method used to show the gui object.
        */
        void show(ImGuiContext* ctx) override;
    };
}