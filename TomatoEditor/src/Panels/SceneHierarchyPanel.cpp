#include "pch.h"
#include "SceneHierarchyPanel.h"

#include <tomato/Scene/EntitySerializer.h>

#include <icons/IconsMaterialDesignIcons.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <string>
#include <tomato/Utils/StringUtils.h>

#include "../EditorLayer.h"
#include "../Utils/UI.h"
#include "../Utils/EditorTheme.h"
#include "tomato/Scene/Entity.h"

namespace tomato
{
	SceneHierarchyPanel::SceneHierarchyPanel(const char* name)
        : BasePanel(name, ICON_MDI_VIEW_LIST, true) { }

	void SceneHierarchyPanel::OnUpdate(Timestep ts)
	{
		const EditorContext& context = EditorLayer::GetInstance()->GetContext();
		m_SelectedEntity = (context.IsValid(EditorContextType::Entity) ? *context.As<Entity*>() : nullptr);

		if (ImGui::IsKeyPressed(ImGuiKey_F2))
		{
			m_RenamingEntity = m_SelectedEntity;
		}
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		
		constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg
			| ImGuiTableFlags_ContextMenuInBody
			| ImGuiTableFlags_BordersInner
			| ImGuiTableFlags_ScrollY;

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });

		if (OnBegin(ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
		{
			float lineHeight = ImGui::GetTextLineHeight();
			ImVec2 padding = ImGui::GetStyle().FramePadding;

			const float filterCursorPosX = ImGui::GetCursorPosX();
			m_Filter.Draw("###HierarchyFilter", ImGui::GetContentRegionAvail().x - (UI::GetIconButtonSize("  " ICON_MDI_PLUS, "Add  ").x + 2.0f * padding.x));
			
			ImGui::SameLine();

			if (UI::IconButton("  " ICON_MDI_PLUS, "Add  "))
            {
                ImGui::OpenPopup("SceneHierarchyContextWindow");
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);
			if (ImGui::BeginPopupContextWindow("SceneHierarchyContextWindow", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
			{
				DrawContextMenu();
				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();

			if (!m_Filter.IsActive())
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(filterCursorPosX + ImGui::GetFontSize() * 0.5f);
				ImGui::TextUnformatted(ICON_MDI_MAGNIFY " Search...");
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::InvisibleButton("##DragDropTargetBehindTable", region);
			DragDropTarget();

			ImGui::SetCursorPos(cursorPos);

			if (ImGui::BeginTable("HierarchyTable", 3, tableFlags))
			{
				ImGui::TableSetupColumn("  Label", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoClip);
				ImGui::TableSetupColumn("  Type", ImGuiTableColumnFlags_WidthFixed, lineHeight * 3.0f);
				ImGui::TableSetupColumn("  " ICON_MDI_EYE_OUTLINE, ImGuiTableColumnFlags_WidthFixed, lineHeight * 2.0f);

				ImGui::TableSetupScrollFreeze(0, 1);
				
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers, ImGui::GetFrameHeight());
				for (int column = 0; column < 3; ++column)
				{
					ImGui::TableSetColumnIndex(column);
					const char* column_name = ImGui::TableGetColumnName(column);
					ImGui::PushID(column);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
					ImGui::TableHeader(column_name);
					ImGui::PopID();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

                for (const auto& entity : m_Scene->m_Objects)
                {
					// TODO 부모만 모아진
                    if (!entity->GetParent())
                    {
						DrawEntityNode(entity);
                    }
                }
				ImGui::PopStyleVar();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);
				if (ImGui::BeginPopupContextWindow("SceneHierarchyContextWindow", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
				{
					EditorLayer::GetInstance()->ResetContext();
					DrawContextMenu();
					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();

				ImGui::EndTable();

				if (ImGui::IsItemClicked())
                {
                    EditorLayer::GetInstance()->ResetContext();
                }
            }

			if (m_DraggedEntity && m_DraggedEntityTarget)
			{
				m_DraggedEntityTarget->AddChild(m_DraggedEntity);
				m_DraggedEntity = {};
				m_DraggedEntityTarget = {};
			}

			if (m_DeletedEntity)
			{
				const EditorContext& context = EditorLayer::GetInstance()->GetContext();
				if (context.IsValid(EditorContextType::Entity) && *context.As<Entity*>() == m_DeletedEntity)
                {
                    EditorLayer::GetInstance()->ResetContext();
                }

				m_Scene->DestroyEntity(m_DeletedEntity);
				m_DeletedEntity = {};
			}

			OnEnd();
		}
		ImGui::PopStyleVar();
	}

	ImRect SceneHierarchyPanel::DrawEntityNode(Entity* entity, uint32_t depth, bool forceExpandTree, bool isPartOfPrefab)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		
		const auto& children = entity->GetChildren();
		if (m_Filter.IsActive() && !m_Filter.PassFilter(entity->GetName().c_str()))
		{
			for (const auto& child : children)
			{
				DrawEntityNode(child);
			}

			return { 0, 0, 0, 0 };
		}
		
		ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanFullWidth;
		flags |= ImGuiTreeNodeFlags_FramePadding;

		if (children.size() == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		bool highlight = m_SelectedEntity == entity;
		if (highlight)
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(EditorTheme::HeaderSelectedColor));
			ImGui::PushStyleColor(ImGuiCol_Header, EditorTheme::HeaderSelectedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorTheme::HeaderSelectedColor);
		}

		if (forceExpandTree)
        {
            ImGui::SetNextItemOpen(true);
        }

        if (!isPartOfPrefab)
        {
			// TODO prefab 컴포
			isPartOfPrefab = false;
        }
        bool prefabColorApplied = isPartOfPrefab && entity != m_SelectedEntity;
		if (prefabColorApplied)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, EditorTheme::HeaderSelectedColor);
        }

        bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(entity->GetUUID())), flags, "%s %s", ICON_MDI_CUBE_OUTLINE, entity->GetName().c_str());

		if (highlight)
        {
            ImGui::PopStyleColor(2);
        }

        // Select
		if (!ImGui::IsItemToggledOpen() &&
			(ImGui::IsItemClicked(ImGuiMouseButton_Left) ||
				ImGui::IsItemClicked(ImGuiMouseButton_Middle) ||
				ImGui::IsItemClicked(ImGuiMouseButton_Right)))
		{
			EditorLayer::GetInstance()->SetContext(EditorContextType::Entity, reinterpret_cast<const char*>(&entity), sizeof(entity));
		}

		// Expand recursively
		if (ImGui::IsItemToggledOpen() && (ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt)))
        {
            forceExpandTree = opened;
        }

        bool entityDeleted = false;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);
		if (ImGui::BeginPopupContextItem())
		{
			if (m_SelectedEntity != entity)
            {
                EditorLayer::GetInstance()->SetContext(EditorContextType::Entity, reinterpret_cast<const char*>(&entity), sizeof(entity));
            }

            if (ImGui::MenuItem("Rename", "F2"))
            {
                m_RenamingEntity = entity;
            }
            if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
            {
                m_Scene->Duplicate(entity);
            }
            if (ImGui::MenuItem("Delete", "Del"))
            {
                entityDeleted = true;
            }

            ImGui::Separator();

			DrawContextMenu();
			
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
		
		ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
		verticalLineStart.x -= 0.5f;
		verticalLineStart.y -= ImGui::GetFrameHeight() * 0.5f;

		// Drag Drop
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* entityPayload = ImGui::AcceptDragDropPayload("Entity"))
				{
					m_DraggedEntity = *static_cast<Entity**>(entityPayload->Data);
					m_DraggedEntityTarget = entity;
				}
				else if (const ImGuiPayload* assetPanelPayload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const char* path = static_cast<char*>(assetPanelPayload->Data);
					std::string ext = StringUtils::GetExtension(path);
					if (ext == "prefab")
					{
						m_DraggedEntity = EntitySerializer::DeserializeEntityAsPrefab(path, *m_Scene);
						m_DraggedEntity = entity;
					}
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Entity", &entity, sizeof(entity));
				ImGui::TextUnformatted(entity->GetName().c_str());
				ImGui::EndDragDropSource();
			}
		}

		if (entity == m_RenamingEntity)
		{
			static bool renaming = false;
			if (!renaming)
			{
				renaming = true;
				ImGui::SetKeyboardFocusHere();
			}

			ImGui::InputText("##Tag", &entity->m_Name);

			if (ImGui::IsItemDeactivated())
			{
				renaming = false;
				m_RenamingEntity = {};
			}
		}

		ImGui::TableNextColumn();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0, 0, 0, 0 });

		float buttonSizeX = ImGui::GetContentRegionAvail().x;
		float frameHeight = ImGui::GetFrameHeight();
		ImGui::Button(isPartOfPrefab ? "Prefab" : "Entity", { buttonSizeX, frameHeight });
		// Select
		if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
        {
            EditorLayer::GetInstance()->SetContext(EditorContextType::Entity, reinterpret_cast<const char*>(&entity), sizeof(entity));
        }

        ImGui::TableNextColumn();
		// Visibility Toggle
		{
			ImGui::Text("  %s", reinterpret_cast<const char*>(entity->IsEnabled() ? ICON_MDI_EYE_OUTLINE : ICON_MDI_EYE_OFF_OUTLINE));
			
            if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
			{
				entity->SetEnabled(!entity->IsEnabled());
			}
		}

		ImGui::PopStyleColor(3);

		if (prefabColorApplied)
        {
            ImGui::PopStyleColor();
        }

        // Open
		const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		{
			if (opened && !entityDeleted)
			{
				ImColor treeLineColor;
				depth %= 4;
				switch (depth)
				{
					case 0:
						treeLineColor = ImColor(254, 112, 246);
						break;
					case 1:
						treeLineColor = ImColor(142, 112, 254);
						break;
					case 2:
						treeLineColor = ImColor(112, 180, 254);
						break;
					case 3:
						treeLineColor = ImColor(48, 134, 198);
						break;
					default:
						treeLineColor = ImColor(255, 255, 255);
						break;
				}

				ImDrawList* drawList = ImGui::GetWindowDrawList();

				ImVec2 verticalLineEnd = verticalLineStart;
				constexpr float lineThickness = 1.5f;

				for (const auto& child : children)
				{
					const float HorizontalTreeLineSize = child->GetChildren().empty() ? 18.0f : 9.0f; //chosen arbitrarily
					const ImRect childRect = DrawEntityNode(child, depth + 1, forceExpandTree, isPartOfPrefab);

					const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
					drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), treeLineColor, lineThickness);
					verticalLineEnd.y = midpoint;
				}
				
				drawList->AddLine(verticalLineStart, verticalLineEnd, treeLineColor, lineThickness);
			}

			if (opened && children.size() > 0)
            {
                ImGui::TreePop();
            }
        }

		// PostProcess Actions
		if(entityDeleted)
        {
            m_DeletedEntity = entity;
        }

        return nodeRect;
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		
		m_Scene = context;
	}

	void SceneHierarchyPanel::DragDropTarget() const
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const char* path = static_cast<char*>(payload->Data);
				std::string name = StringUtils::GetName(path);
				std::string ext = StringUtils::GetExtension(path);

				// TODO Prefab 등 드롭
				/*
				if (ext == "prefab")
				{
					EntitySerializer::DeserializeEntityAsPrefab(path, *m_Scene);
				}
				else if (ext == "arc")
				{
					EditorLayer::GetInstance()->OpenScene(path);
				}
				else if (ext == "hdr")
				{
					m_Scene->CreateEntity(name).AddComponent<SkyLightComponent>()
						.Texture = AssetManager::GetTextureCubemap(path);
				}
				else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp")
				{
					m_Scene->CreateEntity(name).AddComponent<SpriteRendererComponent>()
						.Texture = AssetManager::GetTexture2D(path);
				}
				else if (ext == "assbin" || ext == "obj" || ext == "fbx")
				{
					Ref<Mesh> mesh = AssetManager::GetMesh(path);

					Entity parent = m_Scene->CreateEntity(mesh->GetName());

					size_t meshCount = mesh->GetSubmeshCount();
					if (meshCount == 1)
					{
						auto& meshComponent = parent.AddComponent<MeshComponent>();
						meshComponent.MeshGeometry = mesh;
						meshComponent.SubmeshIndex = 0;
					}
					else
					{
						for (size_t i = 0; i < meshCount; i++)
						{
							const auto& submesh = mesh->GetSubmesh(i);
							Entity entity = m_Scene->CreateEntity(submesh.Name);
							entity.SetParent(parent);
							auto& meshComponent = entity.AddComponent<MeshComponent>();
							meshComponent.MeshGeometry = mesh;
							meshComponent.SubmeshIndex = i;
						}
					}
				}
				*/


			}

			ImGui::EndDragDropTarget();
		}
	}

	void SceneHierarchyPanel::DrawContextMenu() const
	{
		bool hasContext = m_SelectedEntity;

		if (!hasContext)
        {
            EditorLayer::GetInstance()->ResetContext();
        }

        Entity* toSelect = nullptr;
		if (ImGui::MenuItem("Empty Entity"))
		{
			toSelect = m_Scene->CreateEntity(u8"EmptyEntity");
		}/*
		if (ImGui::BeginMenu("2D"))
		{
			if (ImGui::MenuItem("Sprite"))
			{
				toSelect = m_Scene->AddGameObject("Sprite");
				toSelect->AddComponent(new MeshRender);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndMenu();
		}*//*
		if (ImGui::BeginMenu("3D"))
		{
			if (ImGui::MenuItem("Mesh"))
			{
				toSelect = m_Scene->CreateEntity("Mesh");
				toSelect.AddComponent<MeshComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Sky Light"))
			{
				toSelect = m_Scene->CreateEntity("SkyLight");
				toSelect.AddComponent<SkyLightComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Directional Light"))
			{
				toSelect = m_Scene->CreateEntity("DirectionalLight");
				toSelect.AddComponent<LightComponent>().Type = LightComponent::LightType::Directional;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Point Light"))
			{
				toSelect = m_Scene->CreateEntity("PointLight");
				toSelect.AddComponent<LightComponent>().Type = LightComponent::LightType::Point;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Spot Light"))
			{
				toSelect = m_Scene->CreateEntity("SpotLight");
				toSelect.AddComponent<LightComponent>().Type = LightComponent::LightType::Spot;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Audio Source"))
			{
				toSelect = m_Scene->CreateEntity("AudioSource");
				toSelect.AddComponent<AudioSourceComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Audio Listener"))
			{
				toSelect = m_Scene->CreateEntity("AudioListener");
				toSelect.AddComponent<AudioListenerComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Particle System"))
		{
			toSelect = m_Scene->CreateEntity("ParticleSystem");
			toSelect.AddComponent<ParticleSystemComponent>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Camera"))
		{
			toSelect = m_Scene->CreateEntity("Camera");
			toSelect.AddComponent<CameraComponent>();
			ImGui::CloseCurrentPopup();
		}*/

		if (hasContext && toSelect)
        {
			m_SelectedEntity->AddChild(toSelect);
        }

        if (toSelect)
        {
            EditorLayer::GetInstance()->SetContext(EditorContextType::Entity, reinterpret_cast<const char*>(&toSelect), sizeof(toSelect));
        }
    }
}
