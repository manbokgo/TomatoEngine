#pragma once

#include "tomato/Components/AllComponents.h"
#include "tomato/Components/RenderComponent.h"
#include "tomato/Components/Component.h"
#include "tomato/Components/Script.h"

#define GET_COMPONENT(classname) [[nodiscard]] classname##Component* classname() const { return GetComponent<classname##Component>(); }

namespace tomato
{
    using HashCode = size_t;
    struct Collision2DData;

    class Entity
    {
        friend class Scene;
        friend class SceneHierarchyPanel;
        friend class PropertiesPanel;

    public:
        Entity() = default;
        ~Entity() = default;
        CLONE(Entity);

        void OnUpdate();
        void OnLateUpdate();

        void AddChild(Entity* child);
        void RemoveChild(Entity* child);

        void SetParent(Entity* parent);
        void Deparent();

        [[nodiscard]] Entity*                     GetParent() const { return m_Parent; }
        [[nodiscard]] Matrix                      GetWorldTransform() const;
        [[nodiscard]] const vector<Entity*>&      GetChildren() const { return m_Children; }
        [[nodiscard]] const string&               GetName() { return m_Name; }
        [[nodiscard]] const UUID&                 GetUUID() const { return m_UUID; }
        [[nodiscard]] bool                        IsEnabled() const { return m_bEnabled; }
        [[nodiscard]] Scene*                      GetScene() const { return m_Scene; }
        [[nodiscard]] const map<size_t, Script*>& GetScripts() const { return m_Scripts; }

        void SetName(const string& name) { m_Name = name; }
        void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

        void OnSensorExit2D(const Collision2DData& collisionData);
        void OnSensorEnter2D(const Collision2DData& collisionData);
        void OnCollisionExit2D(const Collision2DData& collisionData);
        void OnCollisionEnter2D(const Collision2DData& collisionData);

        template <typename T, typename... Args>
        std::enable_if_t<std::is_base_of_v<Component, T>, T*> AddComponent(Args&&... args)
        {
            constexpr bool isRenderComponent = std::is_base_of_v<RenderComponent, T>;
            if constexpr (isRenderComponent)
            {
                ASSERT(!m_RenderComponent);
            }

            const HashCode hashCode = typeid(T).hash_code();
            ASSERT(!m_Components.count(hashCode));

            Component* comp = new T(args...);
            comp->m_GameObject = this;
            m_Components[hashCode] = comp;

            if constexpr (isRenderComponent)
            {
                m_RenderComponent = (RenderComponent*)comp;
            }

            return static_cast<T*>(comp);
        }

        template <typename T>
        std::enable_if_t<std::is_base_of_v<Component, T>, bool> RemoveComponent()
        {
            const HashCode hashCode = typeid(T).hash_code();

            const auto iter = m_Components.find(hashCode);
            if (iter == m_Components.end())
            {
                return false;
            }

            constexpr bool isRenderComponent = std::is_base_of_v<RenderComponent, T>;
            if constexpr (isRenderComponent)
            {
                ASSERT(m_RenderComponent == iter->second);
                m_RenderComponent = nullptr;
            }

            delete iter->second;
            m_Components.erase(iter);

            return true;
        }

        template <typename T>
        [[nodiscard]] std::enable_if_t<std::is_base_of_v<Component, T>, T*> GetComponent() const
        {
            const HashCode hashCode = typeid(T).hash_code();

            const auto iter = m_Components.find(hashCode);
            if (iter == m_Components.end())
            {
                return nullptr;
            }

            return static_cast<T*>(iter->second);
        }


        template <typename T, typename... Args>
        std::enable_if_t<std::is_base_of_v<Script, T>, T*> AddScript(Args&&... args)
        {
            const HashCode hashCode = typeid(T).hash_code();
            ASSERT(!m_Scripts.count(hashCode));

            Script* comp = new T(args...);
            comp->m_GameObject = this;
            m_Scripts[hashCode] = comp;

            return static_cast<T*>(comp);
        }

        template <typename T>
        std::enable_if_t<std::is_base_of_v<Script, T>, bool> RemoveScript()
        {
            const HashCode hashCode = typeid(T).hash_code();

            const auto iter = m_Scripts.find(hashCode);
            if (iter == m_Scripts.end())
            {
                return false;
            }

            delete iter->second;
            m_Scripts.erase(iter);

            return true;
        }

        // TODO 이거 진짜 맘에 안 든다
        bool RemoveScript(HashCode hashCode)
        {
            const auto iter = m_Scripts.find(hashCode);
            if (iter == m_Scripts.end())
            {
                return false;
            }

            m_Scripts.erase(iter);
            return true;
        }

        template <typename T>
        [[nodiscard]] std::enable_if_t<std::is_base_of_v<Script, T>, T*> GetScript() const
        {
            const HashCode hashCode = typeid(T).hash_code();

            const auto iter = m_Scripts.find(hashCode);
            if (iter == m_Scripts.end())
            {
                return nullptr;
            }

            return static_cast<T*>(iter->second);
        }

        GET_COMPONENT(Transform);
        GET_COMPONENT(MeshRender);
        GET_COMPONENT(Camera);
        GET_COMPONENT(Collider2D);
        GET_COMPONENT(Animator);
        GET_COMPONENT(Light2D);
        GET_COMPONENT(Light3D);
        GET_COMPONENT(ParticleSystem);
        GET_COMPONENT(TileMap);
        GET_COMPONENT(Rigidbody2D);

    private:
        UUID   m_UUID;
        string m_Name;
        Scene* m_Scene = nullptr;

        bool m_bDead = false;
        bool m_bEnabled = true;

        Entity*         m_Parent = nullptr;
        vector<Entity*> m_Children;

        uint16_t m_Layer = BIT(1);

        map<HashCode, Component*> m_Components;
        map<HashCode, Script*>    m_Scripts;
        RenderComponent*          m_RenderComponent = nullptr;
    };
}
