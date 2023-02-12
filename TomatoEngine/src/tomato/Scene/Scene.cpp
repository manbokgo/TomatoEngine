#include "pch.h"
#include "Scene.h"

#pragma warning(push)
#pragma warning(disable : 26495 26498 26813 )
#include <box2d/box2d.h>
#pragma warning(pop)

#include "tomato/Components/CameraComponent.h"
#include "tomato/Components/Collider2DComponent.h"
#include "tomato/Components/RenderComponent.h"
#include "tomato/Components/Rigidbody2DComponent.h"
#include "tomato/Components/TransformComponent.h"
#include "tomato/Renderer/EditorCamera.h"
#include "tomato/Renderer/Renderer3D.h"
#include "tomato/Renderer/Renderer2D.h"
#include "tomato/Renderer/RenderGraphData.h"
#include "tomato/Scene/Entity.h"

namespace tomato
{
    std::map<EntityLayer, EntityLayerData> Scene::LayerCollisionMask =
    {
        {BIT(0), {"Static", static_cast<uint16_t>(0xFFFF), 0}},
        {BIT(1), {"Default", static_cast<uint16_t>(0xFFFF), 1}},
        {BIT(2), {"Player", static_cast<uint16_t>(0xFFFF), 2}},
        {BIT(3), {"Sensor", static_cast<uint16_t>(0xFFFF), 3}},
    };


    #pragma region Physics2DListeners

    class Physics2DContactListener : public b2ContactListener
    {
    public:
        explicit Physics2DContactListener(Scene* scene)
            : m_Scene(scene) { }

        ~Physics2DContactListener() override { }

        Physics2DContactListener(const Physics2DContactListener& other) = delete;
        Physics2DContactListener(Physics2DContactListener&& other) = delete;
        Physics2DContactListener& operator=(const Physics2DContactListener& other) = delete;
        Physics2DContactListener& operator=(Physics2DContactListener&& other) = delete;

        void BeginContact(b2Contact* contact) override
        {
            b2Fixture* a = contact->GetFixtureA();
            b2Fixture* b = contact->GetFixtureB();

            const bool aSensor = a->IsSensor();
            const bool bSensor = b->IsSensor();
            Entity*    e1 = reinterpret_cast<Entity*>(a->GetUserData().pointer);
            Entity*    e2 = reinterpret_cast<Entity*>(b->GetUserData().pointer);

            b2WorldManifold worldManifold{};
            contact->GetWorldManifold(&worldManifold);
            b2Vec2 point = worldManifold.points[0];
            b2Vec2 velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
            b2Vec2 velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            Collision2DData collisionData;
            collisionData.Entity = e2;
            collisionData.RelativeVelocity = Vec2{velocityB.x - velocityA.x, velocityB.y - velocityA.y};

            if (bSensor)
            {
                e1->OnSensorEnter2D(collisionData);
            }
            else
            {
                e1->OnCollisionEnter2D(collisionData);
            }

            point = worldManifold.points[1];
            velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
            velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            collisionData.Entity = e1;
            collisionData.RelativeVelocity = Vec2{velocityA.x - velocityB.x, velocityA.y - velocityB.y};

            if (aSensor)
            {
                e2->OnSensorEnter2D(collisionData);
            }
            else
            {
                e2->OnCollisionEnter2D(collisionData);
            }
        }

        void EndContact(b2Contact* contact) override
        {
            b2Fixture* a = contact->GetFixtureA();
            b2Fixture* b = contact->GetFixtureB();
            const bool aSensor = a->IsSensor();
            const bool bSensor = b->IsSensor();
            Entity*    e1 = reinterpret_cast<Entity*>(a->GetUserData().pointer);
            Entity*    e2 = reinterpret_cast<Entity*>(b->GetUserData().pointer);

            b2WorldManifold worldManifold{};
            contact->GetWorldManifold(&worldManifold);
            b2Vec2 point = worldManifold.points[0];
            b2Vec2 velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
            b2Vec2 velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            Collision2DData collisionData;
            collisionData.Entity = e2;
            collisionData.RelativeVelocity = Vec2{velocityB.x - velocityA.x, velocityB.y - velocityA.y};

            if (bSensor)
            {
                e1->OnSensorExit2D(collisionData);
            }
            else
            {
                e1->OnCollisionExit2D(collisionData);
            }

            point = worldManifold.points[1];
            velocityA = a->GetBody()->GetLinearVelocityFromWorldPoint(point);
            velocityB = b->GetBody()->GetLinearVelocityFromWorldPoint(point);

            collisionData.Entity = e1;
            collisionData.RelativeVelocity = Vec2{velocityA.x - velocityB.x, velocityA.y - velocityB.y};

            if (aSensor)
            {
                e2->OnSensorExit2D(collisionData);
            }
            else
            {
                e2->OnCollisionExit2D(collisionData);
            }
        }

        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
        {
            /* Handle pre solve */
        }

        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
        {
            /* Handle post solve */
        }

        void OnUpdate([[maybe_unused]] Timestep ts) { }

    private:
        Scene* m_Scene;
    };

    #pragma endregion


    Ref<Scene> Scene::Clone() const
    {
        return CreateRef<Scene>(*this);
    }

    Entity* Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity* Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
    {
        auto newEntity = new Entity;
        newEntity->m_UUID = uuid;
        newEntity->m_Name = name;
        newEntity->m_Scene = this;

        m_Objects.push_back(newEntity);
        return newEntity;
    }

    void Scene::DestroyEntity(Entity* entity) {}

    Entity* Scene::Duplicate(Entity* entity)
    {
        auto newEntity = entity->Clone();
        newEntity->m_UUID = UUID();
        m_Objects.push_back(newEntity);
        return newEntity;
    }

    Entity* Scene::GetEntity(UUID uuid)
    {
        for (const auto& object : m_Objects)
        {
            if (object->m_UUID == uuid)
            {
                return object;
            }
        }

        return nullptr;
    }


    void Scene::OnUpdateRuntime([[maybe_unused]] Timestep ts, const Ref<RenderGraphData>& renderGraphData,
                                const EditorCamera*       overrideCamera)
    {
        for (const auto& gameObject : m_Objects)
        {
            gameObject->OnUpdate();
        }

        #pragma region Physics
        {
            // Minimum stable value is 16.0
            constexpr float physicsStepRate = 50.0f;
            constexpr float physicsTs = 1.0f / physicsStepRate;

            bool stepped = false;
            m_PhysicsFrameAccumulator += ts;

            while (m_PhysicsFrameAccumulator >= physicsTs)
            {
                m_ContactListener2D->OnUpdate(physicsTs);
                // TODO GameObject OnFixedUpdate
                m_PhysicsWorld2D->Step(physicsTs, static_cast<int32_t>(VelocityIterations),
                    static_cast<int32_t>(PositionIterations));

                m_PhysicsFrameAccumulator -= physicsTs;
                stepped = true;
            }

            float interpolationFactor = m_PhysicsFrameAccumulator / physicsTs;


            #pragma region Physics2D

            //TODO Objects가 이때는 채워져있어야함.
            for (const auto& e : m_Objects)
            {
                Rigidbody2DComponent* rb = e->Rigidbody2D();
                TransformComponent*   tc = e->Transform();

                if (!rb)
                {
                    continue;
                }

                const b2Body* body = static_cast<b2Body*>(rb->RuntimeBody);
                if (!body->IsAwake())
                {
                    continue;
                }

                if (rb->Interpolation)
                {
                    if (stepped)
                    {
                        b2Vec2 position = body->GetPosition();
                        rb->PreviousTranslationRotation = rb->TranslationRotation;
                        rb->TranslationRotation = Vec3{position.x, position.y, body->GetAngle()};
                    }

                    Vec3 lerpedTranslationRotation = Vec3::Lerp(rb->PreviousTranslationRotation,
                        rb->TranslationRotation,
                        interpolationFactor);
                    tc->m_Position.x = lerpedTranslationRotation.x;
                    tc->m_Position.y = lerpedTranslationRotation.y;
                    tc->m_Rotation.z = lerpedTranslationRotation.z;
                }
                else
                {
                    b2Vec2 position = body->GetPosition();

                    rb->PreviousTranslationRotation = rb->TranslationRotation;
                    rb->TranslationRotation = Vec3{position.x, position.y, body->GetAngle()};

                    tc->m_Position.x = rb->TranslationRotation.x;
                    tc->m_Position.y = rb->TranslationRotation.y;
                    tc->m_Rotation.z = rb->TranslationRotation.z;
                }
            }
        }
        #pragma endregion
        #pragma endregion

        for (const auto& gameObject : m_Objects)
        {
            gameObject->OnLateUpdate();
        }


        #pragma region Audio
        /*{
                auto listenerView = m_Registry.group<AudioListenerComponent>(entt::get<TransformComponent>);
            for (auto&& [e, ac, tc] : listenerView.each())
            {
                if (ac.Active)
                {
                    const glm::mat4 inverted = glm::inverse(Entity(e, this).GetWorldTransform());
                    const glm::vec3 forward = normalize(glm::vec3(inverted[2]));
                    ac.Listener->SetPosition(tc.m_Translation);
                    ac.Listener->SetDirection(-forward);
                    break;
                }
            }

            auto sourceView = m_Registry.group<AudioSourceComponent>(entt::get<TransformComponent>);
            for (auto&& [e, ac, tc] : sourceView.each())
            {
                if (ac.Source)
                {
                    Entity entity = { e, this };
                    const glm::mat4 inverted = glm::inverse(entity.GetWorldTransform());
                    const glm::vec3 forward = normalize(glm::vec3(inverted[2]));
                    ac.Source->SetPosition(tc.m_Translation);
                    ac.Source->SetDirection(forward);
                }
            }
        }*/
        #pragma endregion

        #pragma region VFX
        /*{
                auto particleSystemView = m_Registry.view<TransformComponent, ParticleSystemComponent>();
            for (auto&& [e, tc, psc] : particleSystemView.each())
                psc.System->OnUpdate(ts, tc.m_Translation);
        }*/
        #pragma endregion

        #pragma region Rendering
        CameraData cameraData = {};
        {
            Entity* cameraEntity = GetPrimaryCameraObject();
            if (!overrideCamera)
            {
                if (cameraEntity)
                {
                    cameraData.View = XMMatrixInverse(nullptr, cameraEntity->GetWorldTransform());
                    cameraData.Projection = cameraEntity->Camera()->GetProjection();
                    cameraData.ViewProjection = cameraData.View * cameraData.Projection;
                    cameraData.Position = cameraEntity->Transform()->m_Position;
                }
            }
            else
            {
                cameraData.View = overrideCamera->GetView();
                cameraData.Projection = overrideCamera->GetProjection();
                cameraData.ViewProjection = overrideCamera->GetViewProjection();
                cameraData.Position = overrideCamera->GetPosition();
            }
        }

        OnRender(renderGraphData, cameraData);
        #pragma endregion
    }

    void Scene::OnUpdateEditor(Timestep ts, const Ref<RenderGraphData>& renderGraphData, const EditorCamera& camera)
    {
        for (const auto& gameObject : m_Objects)
        {
            gameObject->OnLateUpdate();
        }

        #pragma region VFX
        /*{
                auto particleSystemView = m_Registry.view<TransformComponent, ParticleSystemComponent>();
            for (auto&& [e, tc, psc] : particleSystemView.each())
                psc.System->OnUpdate(ts, tc.m_Translation);
        }*/
        #pragma endregion

        CameraData cameraData =
        {
            camera.GetView(),
            camera.GetProjection(),
            camera.GetViewProjection(),
            camera.GetPosition()
        };

        // Vec3 a = XMVector3TransformCoord(Vec3(-1.f, 2.5f, 10.f), cameraData.View);
        // Vec3 b = XMVector3TransformCoord(a, cameraData.Projection);
        // Vec3 c = XMVector3TransformCoord(Vec3(-1.f, 2.5f, 10.f), cameraData.ViewProjection);
        // auto d= Vec4::Transform(Vec4(-1.f, 2.5f, 10.f, 1.f), cameraData.ViewProjection);

        OnRender(renderGraphData, cameraData);
    }

    void Scene::OnRender(const Ref<RenderGraphData>& renderGraphData, const CameraData& cameraData)
    {
        std::vector<Entity*> lights;
        {
            /*auto view = m_Registry.view<LightComponent>();
        lights.reserve(view.size());
        for (auto&& [entity, lc] : view.each())
            lights.emplace_back(Entity*(entity, this));*/
        }
        Entity* skylight = {};
        {
            /*auto view = m_Registry.view<SkyLightComponent>();
        if (!view.empty())
            skylight = Entity(*view.begin(), this);*/
        }

        // MeshRenderer::BeginScene(cameraData, skylight, std::move(lights));
        Renderer3D::BeginScene(cameraData);
        {
            for (const auto& e : m_Objects)
            {
                MeshRenderComponent* rc = e->GetComponent<MeshRenderComponent>();
                if (!rc)
                {
                    continue;
                }

                Renderer3D::Draw(rc);
            }
        }
        Renderer3D::EndScene(renderGraphData);

        Renderer2D::BeginScene(cameraData);
        {
            /*auto particleSystemView = m_Registry.view<ParticleSystemComponent>();
        for (auto&& [e, psc] : particleSystemView.each())
            psc.System->OnRender();*/
        }
        {
            for (const auto& e : m_Objects)
            {
                SpriteRenderComponent* rc = e->GetComponent<SpriteRenderComponent>();
                if (!rc)
                {
                    continue;
                }

                Renderer2D::Draw(rc);
            }
        }
        Renderer2D::EndScene(renderGraphData);
    }


    void Scene::OnRuntimeStart()
    {
        SortForSprites();

        m_IsRunning = true;
        m_PhysicsFrameAccumulator = 0.0f;

        #pragma region Physics
        {
            #pragma region Physics2D
            {
                m_PhysicsWorld2D = new b2World({Gravity.x, Gravity.y});
                m_ContactListener2D = new Physics2DContactListener(this);
                m_PhysicsWorld2D->SetContactListener(m_ContactListener2D);

                for (const auto& e : m_Objects)
                {
                    auto rb = e->Rigidbody2D();
                    auto tc = e->Transform();
                    if (rb)
                    {
                        CreateRigidbody2D(e, tc, rb);
                        rb->PreviousTranslationRotation = rb->TranslationRotation = Vec3{
                            tc->m_Position.x, tc->m_Position.y, tc->m_Rotation.z
                        };
                    }
                }
            }
            #pragma endregion
        }
        #pragma endregion

        #pragma region Audio
        { }
        #pragma endregion

        #pragma region VFX
        { }
        #pragma endregion

        #pragma region Scripting
        { }
        #pragma endregion
    }

    void Scene::OnRuntimeStop()
    {
        m_IsRunning = false;

        #pragma region Scripting
        { }
        #pragma endregion

        #pragma region Audio
        { }
        #pragma endregion

        #pragma region Physics
        {
            #pragma region Physics2D
            {
                delete m_ContactListener2D;
                delete m_PhysicsWorld2D;
                m_ContactListener2D = nullptr;
                m_PhysicsWorld2D = nullptr;
            }
            #pragma endregion
        }
        #pragma endregion
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {}
    void Scene::SortForSprites() {}

    void Scene::CreateRigidbody2D(Entity*               entity, const TransformComponent* transform,
                                  Rigidbody2DComponent* component) const
    {
        if (!m_PhysicsWorld2D)
        {
            return;
        }

        b2BodyDef def;
        def.type = static_cast<b2BodyType>(component->Type);
        def.linearDamping = std::max(component->LinearDrag, 0.0f);
        def.angularDamping = std::max(component->AngularDrag, 0.0f);
        def.allowSleep = component->AllowSleep;
        def.awake = component->Awake;
        def.fixedRotation = component->FreezeRotation;
        def.bullet = component->Continuous;
        def.gravityScale = component->GravityScale;

        def.position.Set(transform->m_Position.x, transform->m_Position.y);
        def.angle = transform->m_Rotation.z;

        b2Body* rb = m_PhysicsWorld2D->CreateBody(&def);
        component->RuntimeBody = rb;

        if (auto boxCollider = entity->Collider2D())
        {
            CreateBoxCollider2D(entity, transform, component, boxCollider);
        }

        /*
    if (entity.HasComponent<CircleCollider2DComponent>())
        CreateCircleCollider2D(entity, transform, component, entity.GetComponent<CircleCollider2DComponent>());

    if (entity.HasComponent<PolygonCollider2DComponent>())
        CreatePolygonCollider2D(entity, component, entity.GetComponent<PolygonCollider2DComponent>());
        */

        if (!component->AutoMass && component->Mass > 0.01f)
        {
            b2MassData massData = rb->GetMassData();
            massData.mass = component->Mass;
            rb->SetMassData(&massData);
        }
    }


    void Scene::CreateBoxCollider2D(Entity* entity, const TransformComponent* transform, const Rigidbody2DComponent* rb,
                                    Collider2DComponent* component) const
    {
        if (!m_PhysicsWorld2D)
        {
            return;
        }

        b2PolygonShape boxShape;
        boxShape.SetAsBox(component->Size.x * transform->m_Scale.x, component->Size.y * transform->m_Scale.y,
            {component->Offset.x, component->Offset.y}, 0.0f);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.isSensor = component->IsSensor;
        fixtureDef.density = component->Density;
        fixtureDef.friction = component->Friction;
        fixtureDef.restitution = component->Restitution;
        fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(entity);

        auto layer = entity->m_Layer;
        auto collisionMaskIt = LayerCollisionMask.find(layer);
        if (collisionMaskIt == LayerCollisionMask.end())
        {
            layer = Scene::DefaultLayer;
        }
        fixtureDef.filter.categoryBits = layer;
        fixtureDef.filter.maskBits = LayerCollisionMask[layer].Flags;

        auto*      body = static_cast<b2Body*>(rb->RuntimeBody);
        b2Fixture* fixture = body->CreateFixture(&fixtureDef);
        component->RuntimeFixture = fixture;
    }
}
