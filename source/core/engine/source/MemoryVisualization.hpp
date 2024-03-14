#pragma once
#include <defs/MeasurementObjectDefs.hpp>

namespace core
{
    class MemoryVisualization
        : public GuiControlIfc
    {
        public:
        virtual ~MemoryVisualization() = default;
        /*!
        *  @brief Method used to show the gui object.
        *  @param ctx ImGui context.
        */
        void show(ImGuiContext* ctx);
    };
}