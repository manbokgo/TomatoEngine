#include "pch.h"
#include "EditorLayer.h"

#include <imgui/imgui_internal.h>
#include <icons/IconsMaterialDesignIcons.h>

// #include <tomato/Scene/SceneSerializer.h>
#include <tomato/Utils/PlatformUtils.h>
#include "tomato/Core/Input.h"

#include "Panels/AssetPanel.h"
#include "Panels/ProjectSettingsPanel.h"
#include "Panels/PropertiesPanel.h"
// #include "Panels/RendererSettingsPanel.h"
#include "Panels/SceneViewport.h"
#include "Panels/StatsPanel.h"
#include "tomato/Core/Application.h"
#include "CreateTestLevel.h"
#include "tomato/Renderer/Renderer2D.h"
#include "tomato/Renderer/Renderer3D.h"

#include "Utils/EditorTheme.h"
#include "Utils/UI.h"

namespace tomato
{
    EditorLayer* EditorLayer::s_Instance = nullptr;

    EditorLayer::EditorLayer()
        : Layer("Arc-Editor")
    {
        ASSERT(!s_Instance);

        s_Instance = this;
    }

    void EditorLayer::OnAttach()
    {
        EditorTheme::SetFont();
        EditorTheme::SetStyle();
        EditorTheme::ApplyTheme();

        m_ActiveScene = CreateRef<Scene>();
        m_EditorScene = m_ActiveScene;

        CreateTestLevel(m_ActiveScene);

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        m_Viewports.emplace_back(CreateScope<SceneViewport>());
        m_Viewports[0]->SetContext(m_ActiveScene, m_SceneHierarchyPanel);

        m_Properties.emplace_back(CreateScope<PropertiesPanel>());

        m_AssetPanels.emplace_back(CreateScope<AssetPanel>());

        m_Panels.emplace_back(CreateScope<ProjectSettingsPanel>());
        // m_Panels.emplace_back(CreateScope<RendererSettingsPanel>());
        m_Panels.emplace_back(CreateScope<StatsPanel>());
    }

    void EditorLayer::OnDetach() { }

    void EditorLayer::OnUpdate([[maybe_unused]] Timestep ts)
    {
        Renderer2D::ResetStats();
        Renderer3D::GetInst()->ResetStats();

        {
            // Remove unused scene viewports
            for (auto it = m_Viewports.begin(); it != m_Viewports.end(); ++it)
            {
                if (!it->get()->Showing)
                {
                    it = m_Viewports.erase(it);
                }
            }

            // Remove unused properties panels
            for (auto it = m_Properties.begin(); it != m_Properties.end(); ++it)
            {
                if (!it->get()->Showing)
                {
                    it = m_Properties.erase(it);
                }
            }

            // // Remove unused asset panels
            for (auto it = m_AssetPanels.begin(); it != m_AssetPanels.end(); ++it)
            {
                if (!it->get()->Showing)
                {
                    it = m_AssetPanels.erase(it);
                }
            }
        }

        m_SceneHierarchyPanel.OnUpdate(ts);

        const bool useEditorCamera = m_SceneState == SceneState::Edit ||
            m_SceneState == SceneState::Pause ||
            m_SceneState == SceneState::Step;

        for (const auto& panel : m_Viewports)
        {
            if (panel->Showing)
            {
                panel->OnUpdate(ts);
                panel->SetUseEditorCamera(useEditorCamera);
            }
        }

        for (const auto& panel : m_AssetPanels)
        {
            if (panel->Showing)
            {
                panel->OnUpdate(ts);
            }
        }

        for (const auto& panel : m_Panels)
        {
            if (panel->Showing)
            {
                panel->OnUpdate(ts);
            }
        }
    }

    void EditorLayer::OnImGuiRender()
    {
        ImGuiViewport* const viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport(viewport);


        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoNavFocus;

        ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
        float  frameHeight = ImGui::GetFrameHeight();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        {
            //////////////////////////////////////////////////////////////////////////
            // PRIMARY TOP MENU BAR //////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            ImVec2 framePadding = ImGui::GetStyle().FramePadding;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {framePadding.x, framePadding.y});
            m_TopMenuBarHeight = ImGui::GetFrameHeight();

            if (ImGui::BeginViewportSideBar("##PrimaryMenuBar", viewport, ImGuiDir_Up, m_TopMenuBarHeight,
                window_flags))
            {
                if (ImGui::BeginMenuBar())
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

                    if (ImGui::BeginMenu("File"))
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);

                        if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                        {
                            NewScene();
                        }
                        if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                        {
                            SaveScene();
                        }
                        if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                        {
                            SaveSceneAs();
                        }
                        ImGui::Separator();
                        if (ImGui::BeginMenu("Theme"))
                        {
                            if (ImGui::MenuItem("Light"))
                            {
                                EditorTheme::ApplyTheme(false);
                            }
                            if (ImGui::MenuItem("Dark"))
                            {
                                EditorTheme::ApplyTheme(true);
                            }

                            ImGui::EndMenu();
                        }
                        if (ImGui::MenuItem("Exit"))
                        {
                            Application::Get().Close();
                        }

                        ImGui::PopStyleVar();
                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Window"))
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);

                        if (ImGui::BeginMenu("Add"))
                        {
                            if (ImGui::MenuItem("Viewport"))
                            {
                                size_t index = m_Viewports.size();
                                m_Viewports.emplace_back(CreateScope<SceneViewport>());
                                m_Viewports[index]->SetContext(m_ActiveScene, m_SceneHierarchyPanel);
                            }
                            if (ImGui::MenuItem("Properties"))
                            {
                                m_Properties.emplace_back(CreateScope<PropertiesPanel>());
                            }
                            if (ImGui::MenuItem("Assets"))
                            {
                                m_AssetPanels.emplace_back(CreateScope<AssetPanel>());
                            }

                            ImGui::EndMenu();
                        }
                        ImGui::Separator();
                        ImGui::MenuItem("Hierarchy", nullptr, &m_ShowSceneHierarchyPanel);
                        ImGui::MenuItem(m_ConsolePanel.GetName(), nullptr, &m_ConsolePanel.Showing);

                        for (const auto& panel : m_Panels)
                        {
                            ImGui::MenuItem(panel->GetName(), nullptr, &panel->Showing);
                        }

                        ImGui::Separator();
                        ImGui::MenuItem("ImGui Demo Window", nullptr, &m_ShowDemoWindow);

                        ImGui::PopStyleVar();
                        ImGui::EndMenu();
                    }

                    ImGui::PopStyleVar();

                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {1, 1});

                    {
                        ImVec2 maxRegion = ImGui::GetContentRegionMax();

                        ImVec2            buttonSize = {m_TopMenuBarHeight * 1.5f, m_TopMenuBarHeight};
                        constexpr uint8_t buttonCount = 3;
                        float             buttonStartPositionX = maxRegion.x * 0.5f - buttonCount * 0.5f * buttonSize.x;

                        ImGui::SetCursorPosX(buttonStartPositionX);
                        //Play Button
                        bool highlight = m_SceneState == SceneState::Play || m_SceneState == SceneState::Pause ||
                            m_SceneState == SceneState::Step;
                        const char* icon = m_SceneState == SceneState::Edit ? ICON_MDI_PLAY : ICON_MDI_STOP;
                        if (UI::ToggleButton(icon, highlight, buttonSize))
                        {
                            if (m_SceneState == SceneState::Edit)
                            {
                                OnScenePlay();
                            }
                            else
                            {
                                OnSceneStop();
                            }
                        }

                        //Pause Button
                        highlight = m_SceneState == SceneState::Pause || m_SceneState == SceneState::Step;
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, m_SceneState == SceneState::Edit);
                        if (UI::ToggleButton(ICON_MDI_PAUSE, highlight, buttonSize))
                        {
                            if (m_SceneState == SceneState::Play)
                            {
                                OnScenePause();
                            }
                            else if (m_SceneState == SceneState::Pause)
                            {
                                OnSceneUnpause();
                            }
                        }

                        // Step Button
                        if (m_SceneState == SceneState::Step)
                        {
                            OnScenePause();
                        }
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, m_SceneState != SceneState::Pause);
                        if (ImGui::Button(ICON_MDI_STEP_FORWARD, buttonSize) && m_SceneState == SceneState::Pause)
                        {
                            OnSceneUnpause();
                            m_SceneState = SceneState::Step;
                        }

                        ImGui::PopItemFlag();
                        ImGui::PopItemFlag();

                        ImGui::SetCursorPosX(
                            ImGui::GetContentRegionMax().x - ImGui::CalcTextSize("FPS: XX.XX (XX.XXXms  MEM: XXXX.XXMB")
                            .x);
                        auto   fps = static_cast<double>(ImGui::GetIO().Framerate);
                        size_t allocatedMemory = Application::GetAllocatedMemorySize();
                        ImGui::Text("FPS: %.2lf (%.3lfms)  MEM: %.2lfMB", fps, 1000.0 / fps,
                            static_cast<double>(allocatedMemory) / (1024.0 * 1024.0));
                    }

                    ImGui::PopStyleVar(2);

                    ImGui::EndMenuBar();
                }

                ImGui::End();
            }

            ImGui::PopStyleVar();

            //////////////////////////////////////////////////////////////////////////
            // BOTTOM MENU BAR ///////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            if (ImGui::BeginViewportSideBar("##StatusBar", viewport, ImGuiDir_Down, frameHeight, window_flags))
            {
                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                {
                    m_ConsolePanel.SetFocus();
                }

                if (ImGui::BeginMenuBar())
                {
                    const ConsolePanel::Message* message = m_ConsolePanel.GetRecentMessage();
                    if (message != nullptr)
                    {
                        Vec4 color = ConsolePanel::Message::GetRenderColor(message->Level);
                        ImGui::PushStyleColor(ImGuiCol_Text, color);
                        ImGui::PushFont(EditorTheme::BoldFont);
                        ImGui::TextUnformatted(message->Buffer.c_str());
                        ImGui::PopFont();
                        ImGui::PopStyleColor();
                    }
                    ImGui::EndMenuBar();
                }

                ImGui::End();
            }
        }
        ImGui::PopStyleVar(2);


        //////////////////////////////////////////////////////////////////////////
        // HIERARCHY /////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        if (m_ShowSceneHierarchyPanel)
        {
            m_SceneHierarchyPanel.OnImGuiRender();
        }

        //////////////////////////////////////////////////////////////////////////
        // SCENE VIEWPORTS ///////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        for (const auto& viewportPanel : m_Viewports)
        {
            if (viewportPanel->Showing)
            {
                viewportPanel->OnImGuiRender();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        // ASSETS PANELS /////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        for (const auto& assetPanel : m_AssetPanels)
        {
            if (assetPanel->Showing)
            {
                assetPanel->OnImGuiRender();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        // OTHER PANELS //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        for (const auto& panel : m_Panels)
        {
            if (panel->Showing)
            {
                panel->OnImGuiRender();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        // PROPERTY PANELS ///////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        for (const auto& propertyPanel : m_Properties)
        {
            if (propertyPanel->Showing)
            {
                propertyPanel->SetContext(m_SelectedContext);
                propertyPanel->OnImGuiRender();
            }
        }

        if (m_ConsolePanel.Showing)
        {
            m_ConsolePanel.OnImGuiRender();
        }

        if (m_ShowDemoWindow)
        {
            ImGui::ShowDemoWindow(&m_ShowDemoWindow);
        }

        Application::Get().GetImGuiLayer()->SetBlockEvents(false);


        m_LastMousePosition = ImGui::GetMousePos();
    }

    void EditorLayer::OnEvent([[maybe_unused]] Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(TOMATO_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(TOMATO_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(TOMATO_BIND_EVENT_FN(EditorLayer::OnMouseButtonReleased));
    }
    
    bool EditorLayer::OnKeyPressed([[maybe_unused]] const KeyPressedEvent& e)
    {
        /*
                
                /*
                HotKeys:
                    Entity:
                        Ctrl+Shift+N		New empty entity
                        Alt+Shift+N			New empty child to selected entity
        
                    Edit:
                        Ctrl+D				Duplicate
                        Del					Delete
                        Ctrl+P				Play
                        Ctrl+Shift+P		Pause
                        Ctrl+Alt+P			Step
        
                    Scene:
                        Ctrl+N				Load new scene
                        Ctrl+S				Save current scene
                        Ctrl+Shift+S		Save current scene as
                        Ctrl+Alt+N			New project
                        Ctrl+Alt+O			Open project
                #1#
        
                bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
                bool ctrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
                bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);
        
                switch (e.GetKeyCode())
                {
                    case Key::D:
                    {
                        if (ctrl && m_SelectedContext.GetType() == EditorContextType::Entity)
                        {
                            m_ActiveScene->Duplicate(*m_SelectedContext.As<Entity>());
                            return true;
                        }
                        break;
                    }
                    case Key::P:
                    {
                        if (ctrl && alt && m_SceneState == SceneState::Pause)
                        {
                            OnSceneUnpause();
                            m_SceneState = SceneState::Step;
                            return true;
                        }
                        break;
                    }
                }
        
                if (e.GetRepeatCount() > 0)
                {
                    return false;
                }
        
                switch (e.GetKeyCode())
                {
                    case Key::P:
                    {
                        if (ctrl && shift)
                        {
                            if (m_SceneState == SceneState::Edit)
                            {
                                OnScenePlay();
                                OnScenePause();
                                return true;
                            }
                            if (m_SceneState == SceneState::Play)
                            {
                                OnScenePause();
                                return true;
                            }
                            if (m_SceneState == SceneState::Pause)
                            {
                                OnSceneUnpause();
                                return true;
                            }
                        }
                        if (ctrl)
                        {
                            if (m_SceneState == SceneState::Edit)
                            {
                                OnScenePlay();
                                return true;
                            }
                            
                            OnSceneStop();
                            return true;
                        }
                        break;
                    }
                    case Key::N:
                    {
                        if (ctrl && shift)
                        {
                            Entity entity = m_ActiveScene->CreateEntity();
                            m_SelectedContext.Set(EditorContextType::Entity, reinterpret_cast<const char*>(&entity), sizeof(Entity));
                            return true;
                        }
                        if (ctrl)
                        {
                            NewScene();
                            return true;
                        }
                        if (alt && shift && m_SelectedContext.GetType() == EditorContextType::Entity)
                        {
                            Entity child = m_ActiveScene->CreateEntity();
                            child.SetParent(*m_SelectedContext.As<Entity>());
                            m_SelectedContext.Set(EditorContextType::Entity, reinterpret_cast<const char*>(&child), sizeof(Entity));
                            return true;
                        }
                        break;
                    }
                    case Key::S:
                    {
                        if (ctrl && shift)
                        {
                            SaveSceneAs();
                            return true;
                        }
                        if (ctrl)
                        {
                            SaveScene();
                            return true;
                        }
                        break;
                    }
                    case Key::Delete:
                    {
                        if (GImGui->ActiveId == 0 && m_SelectedContext.GetType() == EditorContextType::Entity)
                        {
                            m_ActiveScene->DestroyEntity(*m_SelectedContext.As<Entity>());
                            m_SelectedContext.Reset();
                            return true;
                        }
                        break;
                    }
                }*/

        return false;
    }

    bool EditorLayer::OnMouseButtonPressed([[maybe_unused]] const MouseButtonPressedEvent& e) const
    {
        return false;
    }

    bool EditorLayer::OnMouseButtonReleased([[maybe_unused]] const MouseButtonReleasedEvent& e) const
    {
        return false;
    }

    void EditorLayer::NewScene()
    {
        /*
                ResetContext();
                if (m_SceneState != SceneState::Edit)
                {
                    OnSceneStop();
                }
        
                for (const auto& propertyPanel : m_Properties)
                {
                    propertyPanel->ForceSetContext(m_SelectedContext);
                }
        
                m_ActiveScene = CreateRef<Scene>();
                m_ActiveScene->MarkViewportDirty();
                m_EditorScene = m_ActiveScene;
        
                m_SceneHierarchyPanel.SetContext(m_ActiveScene);
                if (!m_Viewports.empty())
                {
                    m_Viewports[0]->SetContext(m_ActiveScene, m_SceneHierarchyPanel);
                }
                m_ScenePath = "";
        
                ScriptEngine::SetScene(m_ActiveScene.get());*/
    }

    void EditorLayer::OpenScene(const char* filepath)
    {
        /*
                ResetContext();
                if (m_SceneState != SceneState::Edit)
                {
                    OnSceneStop();
                }
        
                for (const auto& propertyPanel : m_Properties)
                {
                    propertyPanel->ForceSetContext(m_SelectedContext);
                }
        
                m_ActiveScene = CreateRef<Scene>();
                m_ActiveScene->OnViewportResize(1, 1);
                m_ActiveScene->MarkViewportDirty();
                m_EditorScene = m_ActiveScene;
                ScriptEngine::SetScene(m_ActiveScene.get());
        
                m_SceneHierarchyPanel.SetContext(m_ActiveScene);
                if (!m_Viewports.empty())
                {
                    m_Viewports[0]->SetContext(m_ActiveScene, m_SceneHierarchyPanel);
                }
        
                SceneSerializer serializer(m_ActiveScene);
                if (!serializer.Deserialize(filepath))
                {
                    TOMATO_CORE_ERROR("Could not deserialize scene!");
                }
                m_ScenePath = filepath;*/
    }

    void EditorLayer::SaveScene()
    {
        /*
                if (!m_ScenePath.empty())
                {
                    SceneSerializer serializer(m_ActiveScene);
                    serializer.Serialize(m_ScenePath.string());
                }
                else
                {
                    SaveSceneAs();
                }*/
    }

    void EditorLayer::SaveSceneAs()
    {
        /*
                std::string filepath = FileDialogs::SaveFile("Arc Scene (*.arc)\0*.arc\0");
                if (!filepath.empty())
                {
                    SceneSerializer serializer(m_ActiveScene);
                    serializer.Serialize(filepath);
                    m_ScenePath = filepath;
                }*/
    }

    void EditorLayer::OnScenePlay()
    {
        /*
                ResetContext();
        
                m_EditorScene = m_ActiveScene;
                m_RuntimeScene = m_EditorScene->Clone();
                
                m_ActiveScene = m_RuntimeScene;
                ScriptEngine::SetScene(m_ActiveScene.get());
                m_ActiveScene->OnRuntimeStart();
                m_SceneState = SceneState::Play;
        
                m_SceneHierarchyPanel.SetContext(m_ActiveScene);
                if (m_Viewports.empty())
                {
                    m_Viewports.emplace_back(CreateScope<SceneViewport>());
                }
                m_Viewports[0]->SetContext(m_ActiveScene, m_SceneHierarchyPanel);
                m_Viewports[0]->SetSimulation(true);*/
    }

    void EditorLayer::OnSceneStop()
    {
        /*
                ResetContext();
        
                m_ActiveScene->OnRuntimeStop();
                m_SceneState = SceneState::Edit;
        
                m_RuntimeScene = nullptr;
                m_ActiveScene = m_EditorScene;
                ScriptEngine::SetScene(m_ActiveScene.get());
        
                m_SceneHierarchyPanel.SetContext(m_ActiveScene);
                if (!m_Viewports.empty())
                {
                    m_Viewports[0]->SetSimulation(false);
                    m_Viewports[0]->SetContext(m_ActiveScene, m_SceneHierarchyPanel);
                }*/
    }

    void EditorLayer::OnScenePause()
    {
        m_SceneState = SceneState::Pause;

        if (!m_Viewports.empty())
        {
            m_Viewports[0]->SetSimulation(false);
        }
    }

    void EditorLayer::OnSceneUnpause()
    {
        m_SceneState = SceneState::Play;

        if (!m_Viewports.empty())
        {
            m_Viewports[0]->SetSimulation(true);
        }
    }
}
