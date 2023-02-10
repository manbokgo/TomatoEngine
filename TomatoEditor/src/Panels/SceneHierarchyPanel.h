#pragma once

#include "BasePanel.h"
#include "tomato/Core/Timestep.h"

struct ImRect;

namespace tomato
{
    class Entity;
    class Scene;
    class SceneHierarchyPanel : public BasePanel
    {
        friend class SceneHierarchyPanel;

    public:
        explicit SceneHierarchyPanel(const char* name = "Hierarchy");
        ~SceneHierarchyPanel() override = default;

        SceneHierarchyPanel(const SceneHierarchyPanel& other) = delete;
        SceneHierarchyPanel(SceneHierarchyPanel&& other) = delete;
        SceneHierarchyPanel& operator=(const SceneHierarchyPanel& other) = delete;
        SceneHierarchyPanel& operator=(SceneHierarchyPanel&& other) = delete;

        void OnUpdate([[maybe_unused]] Timestep ts) override;
        void OnImGuiRender() override;

        void SetContext(const Ref<Scene>& context);

    private:
        ImRect DrawEntityNode(Entity* entity, uint32_t depth = 0, bool forceExpandTree = false,
                              bool         isPartOfPrefab = false);
        void DrawContextMenu() const;
        void DragDropTarget() const;

        friend class SceneViewport;

    private:
        Ref<Scene> m_Scene = nullptr;
        Entity*    m_SelectedEntity = nullptr;
        Entity*    m_RenamingEntity = nullptr;
        Entity*    m_DeletedEntity = nullptr;
        Entity*    m_DraggedEntity = nullptr;
        Entity*    m_DraggedEntityTarget = nullptr;

        ImGuiTextFilter m_Filter;
    };
}
