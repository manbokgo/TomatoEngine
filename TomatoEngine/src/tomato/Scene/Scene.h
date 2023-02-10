#pragma once

class b2World;
namespace tomato
{
    class Entity;
    class Rigidbody2DComponent;
    class TransformComponent;
    class Collider2DComponent;

    struct RenderGraphData;
    class EditorCamera;
    class Timestep;
    class Physics2DContactListener;
    struct CameraData;

    using EntityLayer = uint16_t;
    struct EntityLayerData
    {
        std::string Name = "Layer";
        EntityLayer Flags = 0xFFFF;
        uint8_t     Index = 1;
    };

    struct Collision2DData
    {
        Entity* Entity = nullptr;
        Vec2    RelativeVelocity = {0.0f, 0.0f};
    };


    class Scene
    {
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;

    public:
        Scene() = default;
        ~Scene() = default;
        [[nodiscard]] Ref<Scene> Clone() const;

        Entity*               CreateEntity(const std::string& name = std::string());
        Entity*               CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void                  DestroyEntity(Entity* entity);
        Entity*               Duplicate(Entity* entity);
        [[nodiscard]] bool    HasEntity(UUID uuid) const;
        [[nodiscard]] Entity* GetEntity(UUID uuid);
        [[nodiscard]] bool    IsRunning() const { return m_IsRunning; }

        void OnUpdateRuntime([[maybe_unused]] Timestep ts, const Ref<RenderGraphData>& renderGraphData,
                             const EditorCamera*       overrideCamera = nullptr);
        void OnUpdateEditor([[maybe_unused]] Timestep ts, const Ref<RenderGraphData>& renderGraphData,
                            const EditorCamera&       camera);
        void OnRender(const Ref<RenderGraphData>& renderGraphData, const CameraData& cameraData);
        void OnRuntimeStart();
        void OnRuntimeStop();

        void                  OnViewportResize(uint32_t width, uint32_t height);
        void                  MarkViewportDirty() { m_ViewportDirty = true; }
        [[nodiscard]] bool    IsViewportDirty() const { return m_ViewportDirty; }
        [[nodiscard]] Entity* GetPrimaryCameraObject() { return m_PrimaryCameraObject; };
        void                  SortForSprites();

    private:
        void CreateRigidbody2D(Entity* entity, const TransformComponent* transform, Rigidbody2DComponent* component) const;
        void CreateBoxCollider2D(Entity*      entity, const TransformComponent* transform, const Rigidbody2DComponent* rb,
                                 Collider2DComponent* component) const;


    public:
        static constexpr uint32_t VelocityIterations = 8;
        static constexpr uint32_t PositionIterations = 3;
        static constexpr Vec2     Gravity = {0.0f, -9.8f};

        static constexpr EntityLayer                  StaticLayer = BIT(0);
        static constexpr EntityLayer                  DefaultLayer = BIT(1);
        static std::map<EntityLayer, EntityLayerData> LayerCollisionMask;

    private:
        vector<Entity*> m_Objects;
        bool            m_IsRunning = false;
        Entity*         m_PrimaryCameraObject;

        b2World*                  m_PhysicsWorld2D = nullptr;
        Physics2DContactListener* m_ContactListener2D = nullptr;
        Vec2                      m_ViewportSize;
        bool                      m_ViewportDirty = true;
        float                     m_PhysicsFrameAccumulator = 0.0f;
    };
}
