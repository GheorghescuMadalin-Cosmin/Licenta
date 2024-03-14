#pragma once
#include <defs/MeasurementObjectDefs.hpp>
#include <statistics/CPUHelper.hpp>
#include <array>


namespace core
{
    /*!
    *   @brief CPU monitor class.
    */
    class CPUMonitor:
        public GuiControlIfc
    {
        static constexpr uint16_t HISTORY_SIZE = 1024; //!< CPU history size.
        core::helper::CPUHelper cpuHelper_; //!< CPU helper.
        std::array<float, HISTORY_SIZE> cpuHistory_; //!< CPU history.
        uint16_t currentIdx_; //!< Current index.
        
    public:
        CPUMonitor(); //!< Default constructor.
        ~CPUMonitor() = default; //!< Default destructor.
        /*!
        *   @brief Update CPU monitor.
        *   @param ctx ImGui context.
        */
        void show(ImGuiContext* ctx) override; 
    };
}