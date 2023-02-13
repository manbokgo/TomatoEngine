#include "pch.h"
#include "PropertiesPanel.h"

#include <icons/IconsMaterialDesignIcons.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <tomato/Scene/EntitySerializer.h>

#include "../Utils/UI.h"
#include "../Utils/EditorTheme.h"

#include "tomato/Components/AllComponents.h"
#include "tomato/Scene/Entity.h"
#include "tomato/Scene/Scene.h"

namespace tomato
{
    PropertiesPanel::PropertiesPanel(const char* name)
        : BasePanel(name, ICON_MDI_INFORMATION, true) { }

    void PropertiesPanel::OnImGuiRender()
    {
        if (OnBegin())
        {
            if (m_Context)
            {
                switch (m_Context.GetType())
                {
                case EditorContextType::None:
                    break;
                case EditorContextType::Entity:
                    {
                        Entity* selectedEntity = *m_Context.As<Entity*>();
                        if (selectedEntity && selectedEntity->GetScene())
                        {
                            DrawComponents(selectedEntity);
                        }
                        else if (m_Locked)
                        {
                            m_Locked = false;
                        }
                        break;
                    }
                case EditorContextType::File:
                    {
                        DrawFileProperties(m_Context.As<char>());
                        break;
                    }
                }
            }
            OnEnd();
        }
    }

    template <typename T, typename UIFunction>
    static void DrawComponent(const char* name, Entity* entity, UIFunction uiFunction, const bool removable = true)
    {
        T* component = entity->GetComponent<T>();
        if (!component)
        {
            return;
        }

        static constexpr ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_AllowItemOverlap
            | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_FramePadding;

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + lineHeight * 0.25f);

        const size_t id = typeid(T).hash_code();
        const bool   open = ImGui::TreeNodeEx(reinterpret_cast<void*>(id), treeFlags, "%s", name);

        bool removeComponent = false;
        if (removable)
        {
            ImGui::PushID(static_cast<int>(id));

            const float frameHeight = ImGui::GetFrameHeight();
            ImGui::SameLine(ImGui::GetContentRegionMax().x - frameHeight * 1.2f);
            if (ImGui::Button(ICON_MDI_SETTINGS, ImVec2{frameHeight * 1.2f, frameHeight}))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    removeComponent = true;
                }

                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        if (open)
        {
            uiFunction(*component);
            ImGui::TreePop();
        }

        if (removeComponent)
        {
            entity->RemoveComponent<T>();
        }
    }

    static void DrawScripts(Entity* entity)
    {
        static constexpr ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_AllowItemOverlap
            | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_FramePadding;

        const auto& scripts = entity->GetScripts();
        HashCode    removeScript = 0;

        for (auto iter = scripts.begin(); iter != scripts.end(); ++iter)
        {
            const size_t hashCode = iter->first;
            Script*      script = iter->second;

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + lineHeight * 0.25f);

            char displayName[128];
            strcpy_s(displayName, ICON_MDI_VECTOR_LINE);
            strcat_s(displayName, script->GetName());

            const bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(hashCode), treeFlags, "%s", displayName);

            {
                ImGui::PushID(static_cast<int>(hashCode));

                const float frameHeight = ImGui::GetFrameHeight();
                ImGui::SameLine(ImGui::GetContentRegionMax().x - frameHeight * 1.2f);
                if (ImGui::Button(ICON_MDI_SETTINGS, ImVec2{frameHeight * 1.2f, frameHeight}))
                {
                    ImGui::OpenPopup("ScriptSettings");
                }

                if (ImGui::BeginPopup("ScriptSettings"))
                {
                    if (ImGui::MenuItem("Remove Script"))
                    {
                        removeScript = hashCode;
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }

            if (open)
            {
                if (UI::BeginProperties())
                {
                    const auto& fields = script->GetScriptFields();
                    for (const auto& field : fields)
                    {
                        const char* header = field.Header.empty() ? nullptr : field.Header.c_str();
                        if (header)
                        {
                            UI::EndProperties();
                            ImGui::Spacing();
                            ImGui::Spacing();
                            ImGui::TextUnformatted(header);
                            ImGui::Spacing();
                            (void)UI::BeginProperties();
                        }

                        UI::DrawField(field);
                    }

                    UI::EndProperties();
                }

                ImGui::TreePop();
            }
        }

        if (removeScript)
        {
            entity->RemoveScript(removeScript);
        }
    }

    /*
    static void DrawMaterialProperties(const Ref<Material>& material)
    {
        UI::BeginProperties();

        const auto& materialProperties = material->GetShader()->GetMaterialProperties();
        for (const auto& [name, property] : materialProperties)
        {
            const char* displayName = property.DisplayName.c_str();

            switch (property.Type)
            {
            case MaterialPropertyType::None: break;
            case MaterialPropertyType::Sampler2D:
                {
                    auto           slot = material->GetData<uint32_t>(name);
                    uint64_t       tex = material->GetTexture(slot) ? material->GetTexture(slot)->GetRendererID() : 0;
                    Ref<Texture2D> tmp = material->GetTexture(slot);
                    if (UI::Property(displayName, tmp, tex))
                    {
                        material->SetTexture(slot, tmp);
                    }
                    break;
                }
            case MaterialPropertyType::Bool:
                {
                    bool v = material->GetData<int32_t>(name) == 0 ? false : true;
                    if (UI::Property(displayName, v))
                    {
                        material->SetData(name, v ? 1 : 0);
                    }
                    break;
                }
            case MaterialPropertyType::Int:
                {
                    auto v = material->GetData<int32_t>(name);
                    if (UI::Property(displayName, v))
                    {
                        material->SetData(name, v);
                    }
                    break;
                }
            case MaterialPropertyType::Float:
                {
                    auto v = material->GetData<float>(name);
                    if (name.ends_with("01"))
                    {
                        if (UI::Property(displayName, v, 0.0f, 1.0f))
                        {
                            material->SetData(name, v);
                        }
                    }
                    else
                    {
                        if (UI::Property(displayName, v))
                        {
                            material->SetData(name, v);
                        }
                    }
                    break;
                }
            case MaterialPropertyType::Float2:
                {
                    auto v = material->GetData<Vec2>(name);
                    if (UI::PropertyVector(displayName, v))
                    {
                        material->SetData(name, v);
                    }
                    break;
                }
            case MaterialPropertyType::Float3:
                {
                    auto v = material->GetData<Vec3>(name);
                    if (UI::PropertyVector(displayName, v, property.IsColor))
                    {
                        material->SetData(name, v);
                    }
                    break;
                }
            case MaterialPropertyType::Float4:
                {
                    auto v = material->GetData<Vec4>(name);
                    if (UI::PropertyVector(displayName, v, property.IsColor))
                    {
                        material->SetData(name, v);
                    }
                    break;
                }
            }
        }

        UI::EndProperties();
    }

    template <typename T>
    static void DrawParticleOverLifetimeModule(std::string_view moduleName, OverLifetimeModule<T>& propertyModule,
                                               bool             color = false, bool                rotation = false)
    {
        static constexpr ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_AllowItemOverlap
            | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_FramePadding;

        if (ImGui::TreeNodeEx(moduleName.data(), treeFlags, "%s", moduleName.data()))
        {
            UI::BeginProperties();
            UI::Property("Enabled", propertyModule.Enabled);

            if (rotation)
            {
                T degrees = glm::degrees(propertyModule.Start);
                if (UI::PropertyVector("Start", degrees))
                {
                    propertyModule.Start = glm::radians(degrees);
                }

                degrees = glm::degrees(propertyModule.End);
                if (UI::PropertyVector("End", degrees))
                {
                    propertyModule.End = glm::radians(degrees);
                }
            }
            else
            {
                UI::PropertyVector("Start", propertyModule.Start, color);
                UI::PropertyVector("End", propertyModule.End, color);
            }

            UI::EndProperties();

            ImGui::TreePop();
        }
    }

    template <typename T>
    static void DrawParticleBySpeedModule(std::string_view moduleName, BySpeedModule<T>& propertyModule,
                                          bool             color = false, bool           rotation = false)
    {
        static constexpr ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_AllowItemOverlap
            | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_FramePadding;

        if (ImGui::TreeNodeEx(moduleName.data(), treeFlags, "%s", moduleName.data()))
        {
            UI::BeginProperties();
            UI::Property("Enabled", propertyModule.Enabled);

            if (rotation)
            {
                T degrees = glm::degrees(propertyModule.Start);
                if (UI::PropertyVector("Start", degrees))
                {
                    propertyModule.Start = glm::radians(degrees);
                }

                degrees = glm::degrees(propertyModule.End);
                if (UI::PropertyVector("End", degrees))
                {
                    propertyModule.End = glm::radians(degrees);
                }
            }
            else
            {
                UI::PropertyVector("Start", propertyModule.Start, color);
                UI::PropertyVector("End", propertyModule.End, color);
            }

            UI::Property("Min Speed", propertyModule.MinSpeed);
            UI::Property("Max Speed", propertyModule.MaxSpeed);
            UI::EndProperties();
            ImGui::TreePop();
        }
    }*/

    void PropertiesPanel::DrawComponents(Entity* entity)
    {
        ImVec2 framePadding = ImGui::GetStyle().FramePadding;
        float  frameHeight = ImGui::GetFrameHeight();

        {
            float  regionX = ImGui::GetContentRegionAvail().x;
            float  addButtonSizeX = UI::GetIconButtonSize("  " ICON_MDI_PLUS, "Add  ").x;
            ImVec2 lockButtonSize = ImVec2(frameHeight * 1.5f, frameHeight);
            float  tagWidth = regionX - ((addButtonSizeX + framePadding.x * 2.0f) + (lockButtonSize.x + framePadding.x *
                2.0f));

            ImGui::SetNextItemWidth(tagWidth);
            ImGui::InputText("##Tag", &entity->m_Name);

            ImGui::SameLine();

            // Add Button
            {
                if (UI::IconButton("  " ICON_MDI_PLUS, "Add  "))
                {
                    ImGui::OpenPopup("AddComponentPopup");
                }

                if (ImGui::BeginPopup("AddComponentPopup"))
                {
                    const float filterCursorPosX = ImGui::GetCursorPosX();
                    m_Filter.Draw("###PropertiesFilter", ImGui::GetContentRegionAvail().x);

                    if (!m_Filter.IsActive())
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(filterCursorPosX + ImGui::GetFontSize() * 0.5f);
                        ImGui::TextUnformatted(ICON_MDI_MAGNIFY " Search...");
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);

                    DrawAddComponent<MeshRenderComponent>(entity, ICON_MDI_IMAGE_SIZE_SELECT_ACTUAL " Sprite Renderer",
                        "2D");
                    DrawAddComponent<Rigidbody2DComponent>(entity, ICON_MDI_SOCCER " Rigidbody 2D", "2D");
                    DrawAddComponent<Collider2DComponent>(entity, ICON_MDI_CHECKBOX_BLANK_OUTLINE " Box Collider 2D",
                        "2D");

                    DrawAddComponent<ParticleSystemComponent>(entity, ICON_MDI_LAMP " Particle System");
                    DrawAddComponent<CameraComponent>(entity, ICON_MDI_CAMERA " Camera");
                    // DrawAddComponent<ScriptComponent>(entity, ICON_MDI_POUND_BOX " Script");

                    ImGui::PopStyleVar();
                    ImGui::EndPopup();
                }
            }

            ImGui::SameLine();

            // Lock Button
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + framePadding.x);
                const char* icon = m_Locked ? ICON_MDI_LOCK : ICON_MDI_LOCK_OPEN_OUTLINE;
                if (UI::ToggleButton(icon, m_Locked, lockButtonSize))
                {
                    m_Locked = !m_Locked;
                }
            }

            ImGui::SetNextItemWidth(regionX - framePadding.x);

            const char* current;
            const auto& it = Scene::LayerCollisionMask.find(entity->m_Layer);
            if (it != Scene::LayerCollisionMask.end())
            {
                current = it->second.Name.c_str();
            }
            else
            {
                current = Scene::LayerCollisionMask[Scene::DefaultLayer].Name.c_str();
            }

            if (ImGui::BeginCombo("##LayerName", current))
            {
                for (const auto& [layer, layerData] : Scene::LayerCollisionMask)
                {
                    bool isSelected = current == layerData.Name;
                    if (ImGui::Selectable(Scene::LayerCollisionMask.at(layer).Name.c_str(), isSelected))
                    {
                        current = Scene::LayerCollisionMask.at(layer).Name.c_str();
                        entity->m_Layer = layer;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }

        ImGui::BeginChild("PropertiesBody");
        DrawComponent<TransformComponent>(ICON_MDI_VECTOR_LINE " Transform", entity, [](TransformComponent& component)
        {
            if (!UI::BeginProperties())
            {
                return;
            }

            UI::DrawVec3Control("Translation", component.m_Position);

            Vec3 rotation = ToDegree(component.m_Rotation);
            // TODO Bool로 변경 확인
            UI::DrawVec3Control("Rotation", rotation);
            component.m_Rotation = ToRadian(rotation);

            UI::DrawVec3Control("Scale", component.m_Scale, nullptr, 1.0f);

            UI::EndProperties();
        }, false);

        DrawComponent<CameraComponent>(ICON_MDI_CAMERA " Camera", entity, [](CameraComponent& component)
        {
                if (!UI::BeginProperties())
                {
                    return;
                }

            UI::Property("Primary", component.m_bPrimary);

            const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
            int         projectionType = static_cast<int>(component.GetProjectionType());
            if (UI::Property("Projection", projectionType, projectionTypeStrings, 2))
            {
                component.SetProjectionType(static_cast<CameraComponent::eProjectionType>(projectionType));
            }

            if (component.GetProjectionType() == CameraComponent::eProjectionType::Perspective)
            {
                float verticalFov = XMConvertToDegrees(component.GetPerspectiveVerticalFOV());
                if (UI::Property("Vertical FOV", verticalFov))
                {
                    component.SetPerspectiveVerticalFOV(XMConvertToRadians(verticalFov));
                }

                float perspectiveNear = component.GetPerspectiveNearClip();
                if (UI::Property("Near Clip", perspectiveNear))
                {
                    component.SetPerspectiveNearClip(perspectiveNear);
                }

                float perspectiveFar = component.GetPerspectiveFarClip();
                if (UI::Property("Far Clip", perspectiveFar))
                {
                    component.SetPerspectiveFarClip(perspectiveFar);
                }
            }

            else if (component.GetProjectionType() == CameraComponent::eProjectionType::Orthographic)
            {
                float orthoSize = component.GetOrthographicSize();
                if (UI::Property("Size", orthoSize))
                {
                    component.SetOrthographicSize(orthoSize);
                }

                float orthoNear = component.GetOrthographicNearClip();
                if (UI::Property("Near Clip", orthoNear))
                {
                    component.SetOrthographicNearClip(orthoNear);
                }

                float orthoFar = component.GetOrthographicFarClip();
                if (UI::Property("Far Clip", orthoFar))
                {
                    component.SetOrthographicFarClip(orthoFar);
                }

                UI::Property("Fixed Aspect Ratio", component.m_bFixedAspectRatio);
            }

            UI::EndProperties();
        });

        /*
        DrawComponent<SpriteRendererComponent>(ICON_MDI_IMAGE_SIZE_SELECT_ACTUAL " Sprite Renderer", entity,
            [&entity](SpriteRendererComponent& component)
            {
                UI::BeginProperties();
                UI::PropertyVector("Color", component.Color, true);
                UI::Property("Texture", component.Texture);
                if (UI::Property("Sorting Order", component.SortingOrder))
                {
                    entity.GetScene()->SortForSprites();
                }
                UI::Property("Tiling Factor", component.TilingFactor);
                UI::EndProperties();
            });

        DrawComponent<MeshComponent>(ICON_MDI_VECTOR_SQUARE " Mesh", entity, [](MeshComponent& component)
        {
            if (ImGui::Button(component.MeshGeometry ? component.MeshGeometry->GetFilepath() : "null", {
                ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight()
            }))
            {
                std::string filepath = FileDialogs::OpenFile(
                    "Mesh (*.assbin)\0*.assbin\0(*.obj)\0*.obj\0(*.fbx)\0*.fbx\0");
                if (!filepath.empty())
                {
                    component.MeshGeometry = CreateRef<Mesh>(filepath.c_str());
                    return;
                }
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    component.MeshGeometry = CreateRef<Mesh>(static_cast<char*>(payload->Data));
                    ImGui::EndDragDropTarget();
                    return;
                }
                ImGui::EndDragDropTarget();
            }

            if (component.MeshGeometry)
            {
                UI::BeginProperties();

                size_t submeshCount = component.MeshGeometry->GetSubmeshCount();
                if (submeshCount > 1)
                {
                    UI::Property<size_t>("Submesh Index", component.SubmeshIndex, 0, submeshCount - 1);
                }

                const char* cullModeTypeStrings[] = {"Front", "Back", "Double Sided"};
                int         cullMode = static_cast<int>(component.CullMode);
                if (UI::Property("Cull Mode", cullMode, cullModeTypeStrings, 3))
                {
                    component.CullMode = static_cast<MeshComponent::CullModeType>(cullMode);
                }
                UI::EndProperties();

                constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
                    | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth |
                    ImGuiTreeNodeFlags_FramePadding;

                if (ImGui::TreeNodeEx("Material Properties", treeNodeFlags))
                {
                    if (component.MeshGeometry->GetSubmeshCount() > 0)
                    {
                        DrawMaterialProperties(component.MeshGeometry->GetSubmesh(component.SubmeshIndex).Mat);
                    }
                    ImGui::TreePop();
                }
            }
        });*/

        /*DrawComponent<ParticleSystemComponent>(ICON_MDI_LAMP " Particle System", entity,
            [](const ParticleSystemComponent& component)
            {
                auto& props = component.System->GetProperties();

                ImGui::Text("Active Particles Count: %u", component.System->GetActiveParticleCount());
                ImGui::BeginDisabled(props.Looping);
                if (ImGui::Button(ICON_MDI_PLAY))
                {
                    component.System->Play();
                }
                ImGui::SameLine();
                if (ImGui::Button(ICON_MDI_STOP))
                {
                    component.System->Stop();
                }
                ImGui::EndDisabled();

                ImGui::Separator();

                UI::BeginProperties();
                UI::Property("Duration", props.Duration);
                if (UI::Property("Looping", props.Looping))
                {
                    if (props.Looping)
                    {
                        component.System->Play();
                    }
                }
                UI::Property("Start Delay", props.StartDelay);
                UI::Property("Start Lifetime", props.StartLifetime);
                UI::PropertyVector("Start Velocity", props.StartVelocity);
                UI::PropertyVector("Start Color", props.StartColor, true);
                UI::PropertyVector("Start Size", props.StartSize);
                UI::PropertyVector("Start Rotation", props.StartRotation);
                UI::Property("Gravity Modifier", props.GravityModifier);
                UI::Property("Simulation Speed", props.SimulationSpeed);
                UI::Property("Play On Awake", props.PlayOnAwake);
                UI::Property("Max Particles", props.MaxParticles);
                UI::EndProperties();

                ImGui::Separator();

                UI::BeginProperties();
                UI::Property("Rate Over Time", props.RateOverTime);
                UI::Property("Rate Over Distance", props.RateOverDistance);
                UI::Property("Burst Count", props.BurstCount);
                UI::Property("Burst Time", props.BurstTime);
                UI::PropertyVector("Position Start", props.PositionStart);
                UI::PropertyVector("Position End", props.PositionEnd);
                UI::Property("Texture", props.Texture);
                UI::EndProperties();

                DrawParticleOverLifetimeModule("Velocity Over Lifetime", props.VelocityOverLifetime);
                DrawParticleOverLifetimeModule("Force Over Lifetime", props.ForceOverLifetime);
                DrawParticleOverLifetimeModule("Color Over Lifetime", props.ColorOverLifetime, true);
                DrawParticleBySpeedModule("Color By Speed", props.ColorBySpeed, true);
                DrawParticleOverLifetimeModule("Size Over Lifetime", props.SizeOverLifetime);
                DrawParticleBySpeedModule("Size By Speed", props.SizeBySpeed);
                DrawParticleOverLifetimeModule("Rotation Over Lifetime", props.RotationOverLifetime, false, true);
                DrawParticleBySpeedModule("Rotation By Speed", props.RotationBySpeed, false, true);
            });*/

        DrawComponent<Rigidbody2DComponent>(ICON_MDI_SOCCER " Rigidbody 2D", entity, [](Rigidbody2DComponent& component)
        {
                if (!UI::BeginProperties())
                {
                    return;
                }

            const char* bodyTypeStrings[] = {"Static", "Kinematic", "Dynamic"};
            int         bodyType = static_cast<int>(component.Type);
            if (UI::Property("Body Type", bodyType, bodyTypeStrings, 3))
            {
                component.Type = static_cast<Rigidbody2DComponent::BodyType>(bodyType);
            }

            if (component.Type == Rigidbody2DComponent::BodyType::Dynamic)
            {
                UI::Property("Auto Mass", component.AutoMass);
                if (!component.AutoMass)
                {
                    UI::Property("Mass", component.Mass, 0.01f, 10000.0f);
                }
                UI::Property("Linear Drag", component.LinearDrag);
                UI::Property("Angular Drag", component.AngularDrag);
                UI::Property("Gravity Scale", component.GravityScale);
                UI::Property("Allow Sleep", component.AllowSleep);
                UI::Property("Awake", component.Awake);
                UI::Property("Continuous", component.Continuous);
                UI::Property("Interpolation", component.Interpolation);
                UI::Property("Freeze Rotation", component.FreezeRotation);

                component.LinearDrag = std::max(component.LinearDrag, 0.0f);
                component.AngularDrag = std::max(component.AngularDrag, 0.0f);
            }

            UI::EndProperties();
        });

        DrawComponent<Collider2DComponent>(ICON_MDI_CHECKBOX_BLANK_OUTLINE " Box Collider 2D", entity,
            [](Collider2DComponent& component)
            {
                if (!UI::BeginProperties())
                {
                    return;
                }

                UI::Property("Is Sensor", component.IsSensor);
                UI::PropertyVector("Size", component.Size);
                UI::PropertyVector("Offset", component.Offset);
                UI::Property("Density", component.Density);
                UI::EndProperties();

                if (!component.IsSensor)
                {
                    ImGui::Spacing();
                    if (!UI::BeginProperties())
                    {
                        return;
                    }

                    UI::Property("Friction", component.Friction);
                    UI::Property("Restitution", component.Restitution);
                    UI::EndProperties();
                }
            });

        DrawScripts(entity);

        ImGui::EndChild();
    }

    void PropertiesPanel::DrawFileProperties(const char* filepath)
    {
        const fs::path path = filepath;
        const fs::path name = path.filename();
        const fs::path ext = path.extension();

        if (ext == "prefab")
        {
            static Entity* prefab;
            if (m_Scene && prefab)
            {
                DrawComponents(prefab);
            }
            else
            {
                m_Scene = CreateRef<Scene>();
                prefab = EntitySerializer::DeserializeEntityAsPrefab(filepath, *m_Scene);
            }
        }
        else
        {
            if (m_Scene)
            {
                m_Scene = nullptr;
            }

            ImGui::TextUnformatted(name.string().c_str());
        }
    }

    template <typename Component>
    void PropertiesPanel::DrawAddComponent(Entity* entity, const char* name, const char* category) const
    {
        const char* displayName = name;
        if (!entity->GetComponent<Component>())
        {
            if (!m_Filter.IsActive())
            {
                if (category)
                {
                    if (ImGui::BeginMenu(category))
                    {
                        if (ImGui::MenuItem(displayName))
                        {
                            entity->AddComponent<Component>();
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndMenu();
                    }
                }
                else
                {
                    if (ImGui::MenuItem(displayName))
                    {
                        entity->AddComponent<Component>();
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            else
            {
                if (m_Filter.IsActive() && m_Filter.PassFilter(displayName))
                {
                    if (ImGui::MenuItem(displayName))
                    {
                        entity->AddComponent<Component>();
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }
    }
}
