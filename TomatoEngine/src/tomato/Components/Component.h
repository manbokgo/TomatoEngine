#pragma once
#define GET_OTHER_COMPONENT_DECLARE(ComponentName) class ComponentName##Component* ComponentName();

namespace tomato
{
    class Component
    {
        friend class Entity;

    public:
        Component();
        Component(const Component& other);
        virtual ~Component();

        virtual void begin() {}
        virtual void OnUpdate() {}
        virtual void OnLateUpdate() {}

        void Activate() { m_bActive = true; }
        void Deactivate() { m_bActive = false; }

        [[nodiscard]] bool    IsActive() const { return m_bActive; }
        [[nodiscard]] Entity* GetGameObject() const { return m_GameObject; }

        GET_OTHER_COMPONENT_DECLARE(Transform);
        GET_OTHER_COMPONENT_DECLARE(MeshRender);
        GET_OTHER_COMPONENT_DECLARE(Camera);
        GET_OTHER_COMPONENT_DECLARE(Collider2D);
        GET_OTHER_COMPONENT_DECLARE(Rigidbody2D);
        GET_OTHER_COMPONENT_DECLARE(Animator);
        GET_OTHER_COMPONENT_DECLARE(Light2D);
        GET_OTHER_COMPONENT_DECLARE(ParticleSystem);
        GET_OTHER_COMPONENT_DECLARE(TileMap);

    private:
        Entity* m_GameObject;
        bool    m_bActive;
    };
}
