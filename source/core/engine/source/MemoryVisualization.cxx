#include "pch.h"
#include "MemoryVisualization.hpp"
#include <statistics/MemoryUsageStatistics.hpp>
#include <imgui_internal.h>
#include <defs/GuiDefs.hpp>

namespace core
{
    void MemoryVisualization::show(ImGuiContext* ctx)
    {
        auto rssMem = core::statistics::getCurrentRSS();
        auto peakRSS = core::statistics::getPeakRSS();
        auto totalMemory = core::statistics::getTotalMemoryAvailable();

        rssMem /= 1024 * 1024;
        peakRSS /= 1024 * 1024;
        totalMemory /= 1024;
        ImGui::SetCurrentContext(ctx);
        
        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
        {
            if(ImGui::TreeNodeEx("Core Memory information", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::Text("RAM memory usage:  %" PRIu64 " MBytes", rssMem);
                ImGui::Text("Peak RAM memory usage:  %" PRIu64 " MBytes", peakRSS);
                ImGui::ProgressBar(float(float(rssMem) / float(totalMemory)), ImVec2(0.0f, 0.0f), (std::to_string(float(float(rssMem) / float(totalMemory)) * 100.0f) + "%").c_str());
                ImGui::TreePop();
            }
            
            ImGui::End();
        }
    }
}