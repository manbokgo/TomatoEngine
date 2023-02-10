#pragma once
#include "tomato/Components/Script.h"

struct ImRect;

namespace tomato
{
    class Entity;
    class UI
    {
    public:
        [[nodiscard]] static bool BeginProperties(
            ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_BordersInner
                | ImGuiTableFlags_BordersOuterH);
        static void EndProperties();

        // Strings
        static bool Property(const char* label, std::string& value, const char* tooltip = nullptr);

        template <typename T>
        static std::enable_if_t<std::is_integral_v<T>, bool> Property(const char* label, T& value, T min = 0, T max = 0,
                                                                      const char* tooltip = nullptr)
        {
            BeginPropertyGrid(label, tooltip);
            bool modified;

            int dataType = ImGuiDataType_S32;
            if constexpr (std::is_signed_v<T>)
            {
                if constexpr (sizeof(T) == 1)
                {
                    dataType = ImGuiDataType_S8;
                }
                else if constexpr (sizeof(T) == 2)
                {
                    dataType = ImGuiDataType_S16;
                }
                else if constexpr (sizeof(T) == 4)
                {
                    dataType = ImGuiDataType_S32;
                }
                else if constexpr (sizeof(T) == 8)
                {
                    dataType = ImGuiDataType_S64;
                }
            }
            else
            {
                if constexpr (sizeof(T) == 1)
                {
                    dataType = ImGuiDataType_U8;
                }
                else if constexpr (sizeof(T) == 2)
                {
                    dataType = ImGuiDataType_U16;
                }
                else if constexpr (sizeof(T) == 4)
                {
                    dataType = ImGuiDataType_U32;
                }
                else if constexpr (sizeof(T) == 8)
                {
                    dataType = ImGuiDataType_U64;
                }
            }

            if (max > min)
            {
                modified = ImGui::SliderScalar(s_IDBuffer, dataType, &value, &min, &max);
            }
            else
            {
                modified = ImGui::DragScalar(s_IDBuffer, dataType, &value);
            }

            EndPropertyGrid();
            return modified;
        }

        template <typename T>
        static std::enable_if_t<std::is_floating_point_v<T>, bool> Property(
            const char* label, T& value, T min = 0, T max = 0, const char* tooltip = nullptr, float delta = 0.1f,
            const char* fmt = "%.3f")
        {
            BeginPropertyGrid(label, tooltip);
            bool modified;

            int dataType = ImGuiDataType_Float;
            if constexpr (sizeof(T) == 8)
            {
                dataType = ImGuiDataType_Double;
            }

            if (max > min)
            {
                modified = ImGui::SliderScalar(s_IDBuffer, dataType, &value, &min, &max, fmt);
            }
            else
            {
                modified = ImGui::DragScalar(s_IDBuffer, dataType, &value, delta, nullptr, nullptr, fmt);
            }

            EndPropertyGrid();
            return modified;
        }

        // Vec2/3/4
        template <typename T>
        static bool PropertyVector(const char* label, T& value, bool color = false, bool showAlpha = true,
                                   const char* tooltip = nullptr, float delta = 0.1f)
        {
            BeginPropertyGrid(label, tooltip);
            bool modified;

            // TODO Hack
            const int componentCount = sizeof(value) / 4;
            if (componentCount >= 3 && color)
            {
                if (showAlpha)
                {
                    modified = ImGui::ColorEdit4(s_IDBuffer, &value.x);
                }
                else
                {
                    modified = ImGui::ColorEdit3(s_IDBuffer, &value.x);
                }
            }
            else
            {
                modified = ImGui::DragScalarN(s_IDBuffer, ImGuiDataType_Float, &value.x, componentCount,
                    delta);
            }
            EndPropertyGrid();
            return modified;
        }

        // Bool
        static bool Property(const char* label, bool& flag, const char* tooltip = nullptr);

        // Dropdown
        static bool Property(const char* label, int& value, const char** dropdownStrings, int count,
                             const char* tooltip = nullptr);

        // 2D/3D Textures
        static bool Property(const char* label, Ref<Texture>& texture, ID3D11ShaderResourceView* overrideTextureID = 0,
                             const char* tooltip = nullptr);

        template <typename T, typename Fn>
        std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> Property(
            const char* label, std::vector<T>& v, T defaultValue = 0, size_t minElements = 0,
            const char* tooltip = nullptr)
        {
            return ListProperty(label, v, defaultValue, minElements, tooltip, [](const char* name, int8_t& value)
            {
                UI::Property(name, value);
            });
        }

        // Vec2/3/4
        static bool Property(const char* label, std::vector<Vec2>&         v,
                             const Vec2& defaultValue = Vec2(0.0f), size_t minElements = 0,
                             const char* tooltip = nullptr);
        static bool Property(const char* label, std::vector<Vec3>&         v,
                             const Vec3& defaultValue = Vec3(0.0f), size_t minElements = 0,
                             const char* tooltip = nullptr);
        static bool Property(const char* label, std::vector<Vec4>&         v,
                             const Vec4& defaultValue = Vec4(0.0f), size_t minElements = 0,
                             const char* tooltip = nullptr);

        // Colors
        static bool PropertyColor(const char* label, std::vector<Vec3>&         v,
                                  const Vec3& defaultValue = Vec3(1.0f), size_t minElements = 0,
                                  const char* tooltip = nullptr);
        static bool PropertyColor(const char* label, std::vector<Vec4>&         v,
                                  const Vec4& defaultValue = Vec4(1.0f), size_t minElements = 0,
                                  const char* tooltip = nullptr);
        static bool PropertyColor4as3(const char* label, std::vector<Vec4>&         v,
                                      const Vec4& defaultValue = Vec4(1.0f), size_t minElements = 0,
                                      const char* tooltip = nullptr);

        // Field
        static void DrawField(const ScriptField& field);

        // Vec3 with reset button
        static void DrawVec3Control(const char* label, Vec3& values, const char* tooltip = nullptr,
                                    float       resetValue = 0.0f);

        // Buttons
        static Vec2 GetIconButtonSize(const char* icon, const char* label);
        static bool IconButton(const char* icon, const char* label,
                               ImVec4 iconColor = {0.537f, 0.753f, 0.286f, 1.0f});
        static bool ToggleButton(const char* label, bool state, ImVec2 size = {0, 0}, float alpha = 1.0f,
                                 float       pressedAlpha = 1.0f, ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_None);

        static void ClippedText(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end,
                                const ImVec2* text_size_if_known, const ImVec2& align, const ImRect* clip_rect,
                                float wrap_width);
        static void ClippedText(ImDrawList*   draw_list, const ImVec2& pos_min, const ImVec2& pos_max, const char* text,
                                const char*   text_display_end, const ImVec2* text_size_if_known, const ImVec2& align,
                                const ImRect* clip_rect, float wrap_width);
        static void AddTextVertical(ImDrawList* DrawList, const char* text, ImVec2 pos, ImU32 text_color);

    private:
        static void PushID();
        static void PopID();

        static void BeginPropertyGrid(const char* label, const char* tooltip, bool rightAlignNextColumn = true);
        static void EndPropertyGrid();

        [[nodiscard]] static bool BeginProperties3(
            ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_BordersInner
                | ImGuiTableFlags_BordersOuterH);

        template <typename T, typename Fn>
        static bool ListProperty(const char* label, std::vector<T>& v, const T& defaultValue, size_t minElements,
                                 const char* tooltip, Fn            function);

    private:
        static char s_IDBuffer[16];
    };
}
