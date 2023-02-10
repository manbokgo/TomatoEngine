#include "pch.h"
#include "BasePanel.h"

#include "../Utils/EditorTheme.h"
#include <fmt/core.h>

namespace tomato
{
    uint32_t BasePanel::s_Count = 0;

    BasePanel::BasePanel(const char* name, const char* icon, bool defaultShow)
        : Showing(defaultShow)
        , m_Name(name)
        , m_Icon(icon)
    {
        m_ID = fmt::format(" {} {}\t\t###{}{}", icon, name, s_Count, name);
        s_Count++;
    }

    bool BasePanel::OnBegin(int32_t windowFlags)
    {
        if (!Showing)
        {
            return false;
        }

        ImGui::SetNextWindowSize(ImVec2(480, 640), ImGuiCond_FirstUseEver);

        ImGui::Begin(m_ID.c_str(), &Showing, windowFlags | ImGuiWindowFlags_NoCollapse);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorTheme::UIFramePadding);
        return true;
    }

    void BasePanel::OnEnd() const
    {
        ImGui::PopStyleVar();
        ImGui::End();
    }
}
