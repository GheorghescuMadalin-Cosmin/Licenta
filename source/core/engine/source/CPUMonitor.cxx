#include "pch.h"
#include "CPUMonitor.hpp"
#include <defs/GuiDefs.hpp>

namespace core
{
    CPUMonitor::CPUMonitor()
    {
        cpuHistory_.fill(0);
        currentIdx_ = 0;
    }

    void CPUMonitor::show(ImGuiContext* ctx)
    {
        ImGui::SetCurrentContext(ctx);
        cpuHistory_[currentIdx_++] = cpuHelper_.getCPUUsage();
        currentIdx_ %= HISTORY_SIZE;
        
        ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginViewportSideBar(OBJECT_TOOLBAR, viewport, ImGuiDir_Right, RIGHT_BAR_WIDTH, window_flags))
        {
            if(ImGui::TreeNodeEx("Core CPU information", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::PlotLines("CPU usage history", cpuHistory_.data(), (int)HISTORY_SIZE, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 50));
                ImGui::TreePop();
            }
            
            ImGui::End();
        }
    }
}