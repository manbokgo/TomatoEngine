#include "pch.h"
#include "UI.h"

#include <icons/IconsMaterialDesignIcons.h>
#include <imgui/imgui_internal.h>

#include "tomato/Components/Script.h"
#include "tomato/Core/CResMgr.h"
#include "tomato/Resources/Texture.h"

namespace tomato
{
    inline static int s_UIContextID = 0;
    inline static int s_Counter = 0;
    char              UI::s_IDBuffer[16];

    void UI::PushID()
    {
        ++s_UIContextID;
        ImGui::PushID(s_UIContextID);
        s_Counter = 0;
    }

    void UI::PopID()
    {
        ImGui::PopID();
        --s_UIContextID;
    }

    void UI::BeginPropertyGrid(const char* label, const char* tooltip, bool rightAlignNextColumn)
    {
        PushID();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::PushID(label);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 0.5f);
        ImGui::TextUnformatted(label);
        if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay))
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(tooltip);
            ImGui::EndTooltip();
        }

        ImGui::TableNextColumn();

        if (rightAlignNextColumn)
        {
            ImGui::SetNextItemWidth(-FLT_MIN);
        }

        s_IDBuffer[0] = '#';
        s_IDBuffer[1] = '#';
        memset(s_IDBuffer + 2, 0, 14);
        ++s_Counter;
        std::string buffer = fmt::format("##{}", s_Counter);
        std::memcpy(&s_IDBuffer, buffer.data(), 16);
    }

    void UI::EndPropertyGrid()
    {
        ImGui::PopID();
        PopID();
    }

    bool UI::BeginProperties(ImGuiTableFlags flags)
    {
        s_IDBuffer[0] = '#';
        s_IDBuffer[1] = '#';
        memset(s_IDBuffer + 2, 0, 14);
        ++s_Counter;
        std::string buffer = fmt::format("##{}", s_Counter);
        std::memcpy(&s_IDBuffer, buffer.data(), 16);

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_PadOuterX;
        if (!ImGui::BeginTable(s_IDBuffer, 2, tableFlags | flags))
        {
            return false;
        }

        ImGui::TableSetupColumn("PropertyName", 0, 0.5f);
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);

        return true;
    }

    bool UI::BeginProperties3(ImGuiTableFlags flags)
    {
        s_IDBuffer[0] = '#';
        s_IDBuffer[1] = '#';
        memset(s_IDBuffer + 2, 0, 14);
        ++s_Counter;
        std::string buffer = fmt::format("##{}", s_Counter);
        std::memcpy(&s_IDBuffer, buffer.data(), 16);

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_PadOuterX;
        if (!ImGui::BeginTable(s_IDBuffer, 3, tableFlags | flags))
        {
            return false;
        }

        ImGui::TableSetupColumn("PropertyName", 0, 0.5f);
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("PropertyExtra", 0, 0.1f);

        return true;
    }

    void UI::EndProperties()
    {
        ImGui::EndTable();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Strings //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool UI::Property(const char* label, std::string& value, const char* tooltip)
    {
        BeginPropertyGrid(label, tooltip);

        bool modified = false;

        // Small strings
        if (value.size() < 255)
        {
            constexpr size_t size = 256;
            char             buffer[size];
            memcpy(buffer, value.data(), size);

            if (ImGui::InputText(s_IDBuffer, buffer, size))
            {
                value = buffer;
                modified = true;
            }
        }
        // Big strings
        else
        {
            const size_t size = value.size() + 256;
            char*        buffer = new char[size];
            memcpy(buffer, value.data(), size);

            if (ImGui::InputText(s_IDBuffer, buffer, size))
            {
                value = buffer;
                modified = true;
            }

            delete[] buffer;
        }

        EndPropertyGrid();

        return modified;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Bool /////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool UI::Property(const char* label, bool& flag, const char* tooltip)
    {
        BeginPropertyGrid(label, tooltip);
        bool modified = ImGui::Checkbox(s_IDBuffer, &flag);
        EndPropertyGrid();
        return modified;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Dropdown /////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    bool UI::Property(const char* label, int& value, const char** dropdownStrings, int count, const char* tooltip)
    {
        BeginPropertyGrid(label, tooltip);

        bool        modified = false;
        const char* current = dropdownStrings[value];

        if (ImGui::BeginCombo(s_IDBuffer, current))
        {
            for (int i = 0; i < count; i++)
            {
                bool isSelected = current == dropdownStrings[i];
                if (ImGui::Selectable(dropdownStrings[i], isSelected))
                {
                    current = dropdownStrings[i];
                    value = i;
                    modified = true;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        UI::EndPropertyGrid();

        return modified;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    /// 2D/3D Textures ///////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool UI::Property(const char* label, Ref<Texture>& texture, ID3D11ShaderResourceView* overrideTextureID,
                      const char* tooltip)
    {
        BeginPropertyGrid(label, tooltip);

        bool changed = false;

        float        frameHeight = ImGui::GetFrameHeight();
        const float  buttonSize = frameHeight * 3.0f;
        const ImVec2 xButtonSize = {buttonSize / 4.0f, buttonSize};
        const float  tooltipSize = frameHeight * 11.0f;

        ImGui::SetCursorPos({
            ImGui::GetContentRegionMax().x - buttonSize - xButtonSize.x,
            ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y
        });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.25f, 0.25f, 0.25f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.35f, 0.35f, 0.35f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.25f, 0.25f, 0.25f, 1.0f});

        ID3D11ShaderResourceView* id = overrideTextureID;
        if (!id)
        {
            // TODO White
            id = !texture ? 0 : texture->GetSRV().Get();
        }
        ImGui::ImageButton(id, {buttonSize, buttonSize}, {1, 1}, {0, 0}, 0);
        if (texture && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay))
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(texture->GetPath().u8string().c_str());
            ImGui::Spacing();
            ImGui::Image(id, {tooltipSize, tooltipSize}, {1, 1}, {0, 0});
            ImGui::EndTooltip();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char* path = static_cast<char*>(payload->Data);
                texture = CResMgr::GetInst()->FindRes<Texture>(path);
                changed = true;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.3f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.2f, 0.2f, 1.0f});
        if (ImGui::Button("x", xButtonSize))
        {
            texture = nullptr;
            changed = true;
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        EndPropertyGrid();

        return changed;
    }


    template <typename T, typename Fn>
    bool UI::ListProperty(const char* label, std::vector<T>& v, const T& defaultValue, size_t minElements,
                          const char* tooltip, Fn            function)
    {
        bool modified = false;
        if (ImGui::TreeNode(label))
        {
            // TODO 점검해야함?
            (void)BeginProperties3();

            size_t pointsCount = v.size();
            size_t step = 1u;
            BeginPropertyGrid("Count", tooltip);
            ImGui::InputScalar(s_IDBuffer, ImGuiDataType_U64, &pointsCount, &step);
            ImGui::TableNextColumn();
            EndPropertyGrid();

            if (pointsCount > v.size())
            {
                v.emplace_back(defaultValue);
                modified = true;
            }
            else if (pointsCount < v.size() && minElements < v.size())
            {
                v.erase(v.end() - 1);
                modified = true;
            }

            bool disabled = pointsCount <= minElements;
            if (disabled)
            {
                ImGui::BeginDisabled();
            }

            int i = 0;
            int removeAt = -1;
            for (T& p : v)
            {
                ImGui::PushID(i);
                std::string name = fmt::format("Point {}", i);
                function(name.c_str(), p);
                ImGui::TableNextColumn();

                if (ImGui::Button(ICON_MDI_CLOSE))
                {
                    removeAt = i;
                }

                ImGui::PopID();

                ++i;
            }

            if (disabled)
            {
                ImGui::EndDisabled();
            }

            EndProperties();

            if (removeAt > -1)
            {
                v.erase(v.begin() + removeAt);
            }

            ImGui::TreePop();
        }

        return modified;
    }

    bool UI::Property(const char* label, std::vector<Vec2>& v, const Vec2& defaultValue, size_t minElements,
                      const char* tooltip)
    {
        return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, Vec2& value)
        {
            PropertyVector(name, value);
        });
    }

    bool UI::Property(const char* label, std::vector<Vec3>& v, const Vec3& defaultValue, size_t minElements,
                      const char* tooltip)
    {
        return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, Vec3& value)
        {
            PropertyVector(name, value);
        });
    }

    bool UI::Property(const char* label, std::vector<Vec4>& v, const Vec4& defaultValue, size_t minElements,
                      const char* tooltip)
    {
        return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, Vec4& value)
        {
            UI::PropertyVector(name, value);
        });
    }

    bool UI::PropertyColor(const char* label, std::vector<Vec3>& v, const Vec3& defaultValue, size_t minElements,
                           const char* tooltip)
    {
        return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, Vec3& value)
        {
            UI::PropertyVector(name, value, true);
        });
    }

    bool UI::PropertyColor(const char* label, std::vector<Vec4>& v, const Vec4& defaultValue, size_t minElements,
                           const char* tooltip)
    {
        return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, Vec4& value)
        {
            UI::PropertyVector(name, value, true);
        });
    }

    bool UI::PropertyColor4as3(const char* label, std::vector<Vec4>& v, const Vec4& defaultValue, size_t minElements,
                               const char* tooltip)
    {
        return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, Vec4& value)
        {
            UI::PropertyVector(name, value, true, false);
        });
    }

    void UI::DrawField(const ScriptField& field)
    {
        if (field.Type == FieldType::Unknown)
        {
            return;
        }

        const char* tooltip = field.Tooltip.empty() ? nullptr : field.Tooltip.c_str();

        switch (field.Type)
        {
        case FieldType::Unknown:
            break;
        case FieldType::Bool:
            {
                UI::Property(field.Name.c_str(), *(bool*)field.Field, tooltip);
                break;
            }
        case FieldType::Float:
            {
                UI::Property(field.Name.c_str(), *(float*)field.Field, field.Min, field.Max, tooltip);
                break;
            }
        case FieldType::Double:
            {
                UI::Property(field.Name.c_str(), *(double*)field.Field, (double)field.Min, (double)field.Max, tooltip);
                break;
            }
        case FieldType::Byte:
            {
                UI::Property(field.Name.c_str(), *(int8_t*)field.Field, (int8_t)field.Min, (int8_t)field.Max, tooltip);
                break;
            }
        case FieldType::UByte:
            {
                UI::Property(field.Name.c_str(), *(uint8_t*)field.Field, (uint8_t)field.Min, (uint8_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::Short:
            {
                UI::Property(field.Name.c_str(), *(int16_t*)field.Field, (int16_t)field.Min, (int16_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::UShort:
            {
                UI::Property(field.Name.c_str(), *(uint16_t*)field.Field, (uint16_t)field.Min, (uint16_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::Int:
            {
                UI::Property(field.Name.c_str(), *(int32_t*)field.Field, (int32_t)field.Min, (int32_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::UInt:
            {
                UI::Property(field.Name.c_str(), *(uint32_t*)field.Field, (uint32_t)field.Min, (uint32_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::Long:
            {
                UI::Property(field.Name.c_str(), *(int64_t*)field.Field, (int64_t)field.Min, (int64_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::ULong:
            {
                UI::Property(field.Name.c_str(), *(uint64_t*)field.Field, (uint64_t)field.Min, (uint64_t)field.Max,
                    tooltip);
                break;
            }
        case FieldType::String:
            {
                UI::Property(field.Name.c_str(), *(string*)field.Field, tooltip);
                break;
            }
        case FieldType::Vector2:
            {
                UI::PropertyVector(field.Name.c_str(), *(Vec2*)field.Field, false, true, tooltip);
                break;
            }
        case FieldType::Vector3:
            {
                UI::PropertyVector(field.Name.c_str(), *(Vec3*)field.Field, false, true, tooltip);
                break;
            }
        case FieldType::Vector4:
            {
                UI::PropertyVector(field.Name.c_str(), *(Vec4*)field.Field, false, true, tooltip);
                break;
            }
        case FieldType::Color:
            {
                UI::PropertyVector(field.Name.c_str(), *(Vec4*)field.Field, true, true, tooltip);
                break;
            }
        case FieldType::Char:
            break;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Vec3 with reset button ///////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    void UI::DrawVec3Control(const char* label, Vec3& values, const char* tooltip, float resetValue)
    {
        BeginPropertyGrid(label, tooltip, false);

        ImGuiIO& io = ImGui::GetIO();
        auto     boldFont = io.Fonts->Fonts[1];

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

        float  frameHeight = ImGui::GetFrameHeight();
        ImVec2 buttonSize = {frameHeight + 3.0f, frameHeight};

        ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

        // X
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize))
            {
                values.x = resetValue;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        // Y
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize))
            {
                values.y = resetValue;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        // Z
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Z", buttonSize))
            {
                values.z = resetValue;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar();

        EndPropertyGrid();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Buttons //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool UI::IconButton(const char* icon, const char* label, ImVec4 iconColor)
    {
        PushID();

        float  lineHeight = ImGui::GetTextLineHeight();
        ImVec2 padding = ImGui::GetStyle().FramePadding;

        float width = ImGui::CalcTextSize(icon).x;
        width += ImGui::CalcTextSize(label).x;
        width += padding.x * 2.0f;

        const float cursorPosX = ImGui::GetCursorPosX();
        bool        clicked = ImGui::Button(s_IDBuffer, {width, lineHeight + padding.y * 2.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
        ImGui::SameLine();
        ImGui::SetCursorPosX(cursorPosX);
        ImGui::TextColored(iconColor, "%s", icon);
        ImGui::SameLine();
        ImGui::TextUnformatted(label);
        ImGui::PopStyleVar();
        PopID();

        return clicked;
    }

    Vec2 UI::GetIconButtonSize(const char* icon, const char* label)
    {
        float  lineHeight = ImGui::GetTextLineHeight();
        ImVec2 padding = ImGui::GetStyle().FramePadding;

        float width = ImGui::CalcTextSize(icon).x;
        width += ImGui::CalcTextSize(label).x;
        width += padding.x * 2.0f;

        return Vec2(width, lineHeight + padding.y * 2.0f);
    }

    bool UI::ToggleButton(const char*      label, bool state, ImVec2 size, float alpha, float pressedAlpha,
                          ImGuiButtonFlags buttonFlags)
    {
        if (state)
        {
            ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];

            color.w = pressedAlpha;
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        }
        else
        {
            ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_Button];
            ImVec4 hoveredColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
            color.w = alpha;
            hoveredColor.w = pressedAlpha;
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
            color.w = pressedAlpha;
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        }

        bool clicked = ImGui::ButtonEx(label, size, buttonFlags);

        ImGui::PopStyleColor(3);

        return clicked;
    }

    void UI::ClippedText(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end,
                         const ImVec2* text_size_if_known, const ImVec2& align, const ImRect* clip_rect,
                         float         wrap_width)
    {
        // Hide anything after a '##' string
        const char* text_display_end = ImGui::FindRenderedTextEnd(text, text_end);
        const int   text_len = static_cast<int>(text_display_end - text);
        if (text_len == 0)
        {
            return;
        }

        ImGuiContext& g = *GImGui;
        ImGuiWindow*  window = g.CurrentWindow;
        UI::ClippedText(window->DrawList, pos_min, pos_max, text, text_display_end, text_size_if_known, align,
            clip_rect, wrap_width);
        if (g.LogEnabled)
        {
            ImGui::LogRenderedText(&pos_min, text, text_display_end);
        }
    }

    void UI::ClippedText(ImDrawList*   draw_list, const ImVec2&        pos_min, const ImVec2& pos_max, const char* text,
                         const char*   text_display_end, const ImVec2* text_size_if_known, const ImVec2& align,
                         const ImRect* clip_rect, float                wrap_width)
    {
        // Perform CPU side clipping for single clipped element to avoid using scissor state
        ImVec2       pos = pos_min;
        const ImVec2 text_size = text_size_if_known ? *text_size_if_known
                                     : ImGui::CalcTextSize(text, text_display_end, false, wrap_width);

        const ImVec2* clip_min = clip_rect ? &clip_rect->Min : &pos_min;
        const ImVec2* clip_max = clip_rect ? &clip_rect->Max : &pos_max;

        // Align whole block. We should defer that to the better rendering function when we'll have support for individual line alignment.
        if (align.x > 0.0f)
        {
            pos.x = ImMax(pos.x, pos.x + (pos_max.x - pos.x - text_size.x) * align.x);
        }

        if (align.y > 0.0f)
        {
            pos.y = ImMax(pos.y, pos.y + (pos_max.y - pos.y - text_size.y) * align.y);
        }

        // Render
        ImVec4 fine_clip_rect(clip_min->x, clip_min->y, clip_max->x, clip_max->y);
        draw_list->AddText(nullptr, 0.0f, pos, ImGui::GetColorU32(ImGuiCol_Text), text, text_display_end, wrap_width,
            &fine_clip_rect);
    }

    /// Draws vertical text. The position is the bottom left of the text rect.
    void UI::AddTextVertical(ImDrawList* DrawList, const char* text, ImVec2 pos, ImU32 text_color)
    {
        pos.x = IM_ROUND(pos.x);
        pos.y = IM_ROUND(pos.y);
        const ImFont* font = GImGui->Font;
        char          c;
        while ((c = *text++))
        {
            const ImFontGlyph* glyph = font->FindGlyph(c);
            if (!glyph)
            {
                continue;
            }

            DrawList->PrimReserve(6, 4);
            DrawList->PrimQuadUV(
                pos + ImVec2(glyph->Y0, -glyph->X0),
                pos + ImVec2(glyph->Y0, -glyph->X1),
                pos + ImVec2(glyph->Y1, -glyph->X1),
                pos + ImVec2(glyph->Y1, -glyph->X0),

                ImVec2(glyph->U0, glyph->V0),
                ImVec2(glyph->U1, glyph->V0),
                ImVec2(glyph->U1, glyph->V1),
                ImVec2(glyph->U0, glyph->V1),
                text_color);
            pos.y -= glyph->AdvanceX;
        }
    }
}
