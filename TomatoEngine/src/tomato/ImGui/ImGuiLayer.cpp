#include "pch.h"
#include "ImGuiLayer.h"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include <imgui.h>
#include <ImGuizmo.h>

#include "tomato/Core/Application.h"
#include "tomato/Renderer/Device.h"

namespace tomato
{
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer") { }

    void ImGuiLayer::OnAttach()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigDragClickToInputText = true;
        io.ConfigDockingTransparentPayload = true;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        // Setup Platform/Renderer bindings
        ImGui_ImplWin32_Init(Application::Get().GetWindowHandle());
        ImGui_ImplDX11_Init(DEVICE, CONTEXT);

        {
            ImGuizmo::Style* imguizmoStyle = &ImGuizmo::GetStyle();
            ImVec4*          colors = imguizmoStyle->Colors;

            imguizmoStyle->TranslationLineThickness = 3.0f;
            imguizmoStyle->TranslationLineArrowSize = 10.0f;
            imguizmoStyle->RotationLineThickness = 3.0f;
            imguizmoStyle->RotationOuterLineThickness = 5.0f;
            imguizmoStyle->ScaleLineThickness = 3.0f;
            imguizmoStyle->ScaleLineCircleSize = 8.0f;
            imguizmoStyle->HatchedAxisLineThickness = 0.0f;
            imguizmoStyle->CenterCircleSize = 6.0f;

            colors[ImGuizmo::DIRECTION_X] = ImVec4(0.858f, 0.243f, 0.113f, 0.929f);
            colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.375f, 0.825f, 0.372f, 0.929f);
            colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.227f, 0.478f, 0.972f, 0.929f);
            colors[ImGuizmo::PLANE_X] = colors[ImGuizmo::DIRECTION_X];
            colors[ImGuizmo::PLANE_Y] = colors[ImGuizmo::DIRECTION_Y];
            colors[ImGuizmo::PLANE_Z] = colors[ImGuizmo::DIRECTION_Z];

            ImGuizmo::AllowAxisFlip(false);
        }
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnEvent([[maybe_unused]] Event& e)
    {
        if (m_BlockEvents)
        {
            const ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }

    void ImGuiLayer::Begin() const
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiLayer::End() const
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}
