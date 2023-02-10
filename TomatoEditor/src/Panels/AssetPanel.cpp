#include "pch.h"
#include "AssetPanel.h"

// #include <tomato/Scene/EntitySerializer.h>
// #include <tomato/Core/Filesystem.h>

#include <icons/IconsMaterialDesignIcons.h>
#include <imgui/imgui_internal.h>
#include <tomato/Utils/PlatformUtils.h>

#include "../EditorLayer.h"
#include "../Utils/UI.h"
#include "../Utils/EditorTheme.h"
#include "../Utils/FileWatch.hpp"
#include "tomato/Core/CResMgr.h"
#include "tomato/Resources/Texture.h"
#include "tomato/Utils/StringUtils.h"

namespace tomato
{
    static const unordered_map<FileType, const char*> s_FileTypesToString =
    {
        {FileType::Unknown, "Unknown"},

        {FileType::Scene, "Scene"},
        {FileType::Prefab, "Prefab"},
        {FileType::Shader, "Shader"},

        {FileType::Texture, "Texture"},
        {FileType::Cubemap, "Cubemap"},
        {FileType::Model, "Model"},

        {FileType::Audio, "Audio"},
    };

    static const unordered_map<std::string, FileType> s_FileTypes =
    {
        {".arc", FileType::Scene},
        {".prefab", FileType::Prefab},
        {".glsl", FileType::Shader},

        {".png", FileType::Texture},
        {".jpg", FileType::Texture},
        {".jpeg", FileType::Texture},
        {".bmp", FileType::Texture},
        {".gif", FileType::Texture},

        {".hdr", FileType::Cubemap},
        {".tga", FileType::Cubemap},

        {".obj", FileType::Model},
        {".fbx", FileType::Model},
        {".gltf", FileType::Model},

        {".mp3", FileType::Audio},
        {".m4a", FileType::Audio},
        {".wav", FileType::Audio},
    };

    static const unordered_map<FileType, ImVec4> s_TypeColors =
    {
        {FileType::Scene, {0.75f, 0.35f, 0.20f, 1.00f}},
        {FileType::Prefab, {0.10f, 0.50f, 0.80f, 1.00f}},
        {FileType::Shader, {0.10f, 0.50f, 0.80f, 1.00f}},

        {FileType::Texture, {0.80f, 0.20f, 0.30f, 1.00f}},
        {FileType::Cubemap, {0.80f, 0.20f, 0.30f, 1.00f}},
        {FileType::Model, {0.20f, 0.80f, 0.75f, 1.00f}},

        {FileType::Audio, {0.20f, 0.80f, 0.50f, 1.00f}},
    };

    static const unordered_map<FileType, const char*> s_FileTypesToIcon =
    {
        {FileType::Unknown, ICON_MDI_FILE},

        {FileType::Scene, ICON_MDI_FILE},
        {FileType::Prefab, ICON_MDI_FILE},
        {FileType::Shader, ICON_MDI_IMAGE_FILTER_BLACK_WHITE},

        {FileType::Texture, ICON_MDI_FILE_IMAGE},
        {FileType::Cubemap, ICON_MDI_IMAGE_FILTER_HDR},
        {FileType::Model, ICON_MDI_VECTOR_POLYGON},

        {FileType::Audio, ICON_MDI_MICROPHONE},
    };

    inline static float s_LastDomainReloadTime = 0.0f;

    static bool DragDropTarget(const std::filesystem::path& dropPath)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
            {
                // Entity entity = *static_cast<Entity*>(payload->Data);
                // std::filesystem::path path = dropPath / std::string(entity.GetComponent<TagComponent>().Tag + ".prefab");
                // EntitySerializer::SerializeEntityAsPrefab(path.string().c_str(), entity);
                return true;
            }

            ImGui::EndDragDropTarget();
        }

        return false;
    }

    static void DragDropFrom(const std::filesystem::path& filepath)
    {
        if (ImGui::BeginDragDropSource())
        {
            const std::string pathStr = filepath.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
            ImGui::TextUnformatted(filepath.filename().string().c_str());
            ImGui::EndDragDropSource();
        }
    }

    static void OpenFile(const std::filesystem::path& path)
    {
        std::string filepathString = path.string();
        const char* filepath = filepathString.c_str();
        std::string ext = path.extension().string();
        const auto& fileTypeIt = s_FileTypes.find(ext);
        if (fileTypeIt != s_FileTypes.end())
        {
            FileType fileType = fileTypeIt->second;
            switch (fileType)
            {
            case FileType::Scene:
                EditorLayer::GetInstance()->OpenScene(filepath);
                break;
            case FileType::Shader:
                break;
            case FileType::Unknown:
            case FileType::Prefab:
            case FileType::Texture:
            case FileType::Cubemap:
            case FileType::Model:
            case FileType::Audio:
                // FileDialogs::OpenFileWithProgram(filepath);
                break;
            }
        }
        else
        {
            // FileDialogs::OpenFileWithProgram(filepath);
        }
    }

    std::pair<bool, fs::path> AssetPanel::DirectoryTreeViewRecursive(const std::filesystem::path& path,
                                                                     vector<fs::path>&            selectionMask,
                                                                     ImGuiTreeNodeFlags           flags)
    {
        bool     anyNodeClicked = false;
        fs::path nodeClicked;

        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            ImGuiTreeNodeFlags nodeFlags = flags;

            auto& entryPath = entry.path();
            const std::string filepath = entryPath.string();

            const bool entryIsFile = entry.is_regular_file();
            if (entryIsFile)
            {
                nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            
            auto iter = std::find(selectionMask.begin(), selectionMask.end(), entryPath);
            const bool selected = iter != selectionMask.end();
            if (selected)
            {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
                ImGui::PushStyleColor(ImGuiCol_Header, EditorTheme::HeaderSelectedColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorTheme::HeaderSelectedColor);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorTheme::HeaderHoveredColor);
            }

            const bool open = ImGui::TreeNodeEx(entryPath.string().c_str(), nodeFlags, "");
            ImGui::PopStyleColor(selected ? 2 : 1);

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                if (!entryIsFile)
                {
                    UpdateDirectoryEntries(entryPath);
                }

                nodeClicked = entryPath;
                anyNodeClicked = true;
            }

            if (!entryIsFile)
            {
                DragDropTarget(entryPath);
            }
            DragDropFrom(entryPath);

            std::string name = StringUtils::GetNameWithExtension(filepath);

            const char* folderIcon = ICON_MDI_FILE;
            if (entryIsFile)
            {
                auto        fileType = FileType::Unknown;
                const auto& fileTypeIt = s_FileTypes.find(entryPath.extension().string());
                if (fileTypeIt != s_FileTypes.end())
                {
                    fileType = fileTypeIt->second;
                }

                const auto& fileTypeIconIt = s_FileTypesToIcon.find(fileType);
                if (fileTypeIconIt != s_FileTypesToIcon.end())
                {
                    folderIcon = fileTypeIconIt->second;
                }
            }
            else
            {
                folderIcon = open ? ICON_MDI_FOLDER_OPEN : ICON_MDI_FOLDER;
            }

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, EditorTheme::AssetIconColor);
            ImGui::TextUnformatted(folderIcon);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::TextUnformatted(name.c_str());
            m_CurrentlyVisibleItemsTreeView++;
            
            if (!entryIsFile)
            {
                if (open)
                {
                    const auto [isClicked, clickedNode] = DirectoryTreeViewRecursive(entryPath, selectionMask, flags);

                    if (!anyNodeClicked)
                    {
                        anyNodeClicked = isClicked;
                        nodeClicked = clickedNode;
                    }

                    ImGui::TreePop();
                }
            }
        }

        return {anyNodeClicked, nodeClicked};
    }

    AssetPanel::AssetPanel(const char* name)
        : BasePanel(name, ICON_MDI_FOLDER_STAR, true)
    {
        m_WhiteTexture = CreateRef<Texture>();
        m_WhiteTexture->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET);

        m_DirectoryIcon = CResMgr::GetInst()->LoadRelative<Texture>("Resources/Icons/ContentBrowser/DirectoryIcon.png");
        m_FileIcon = CResMgr::GetInst()->LoadRelative<Texture>("Resources/Icons/ContentBrowser/FileIcon.png");

        m_AssetsDirectory = CPathMgr::GetInst()->GetContentPath();
        m_CurrentDirectory = m_AssetsDirectory;
        Refresh();

        static filewatch::FileWatch<std::string> watch(m_AssetsDirectory.string(),
            [this](const auto&                   path, const filewatch::Event change_type)
            {
                if (s_LastDomainReloadTime < 0.1f)
                {
                    return;
                }

                Refresh();

                std::filesystem::path filepath = path;
                std::string           ext = filepath.extension().string();

                switch (change_type)
                {
                case filewatch::Event::added:
                    TOMATO_CORE_TRACE("The file was added to the directory: {}", filepath);
                    break;
                case filewatch::Event::removed:
                    TOMATO_CORE_TRACE("The file was removed from the directory: {}", filepath);
                    break;
                case filewatch::Event::modified:
                    TOMATO_CORE_TRACE("The file was modified: {}; This can be a change in the time stamp or attributes.",
                        filepath);
                    break;
                case filewatch::Event::renamed_old:
                    TOMATO_CORE_TRACE("The file was renamed and this is the old name: {}", filepath);
                    break;
                case filewatch::Event::renamed_new:
                    TOMATO_CORE_TRACE("The file was renamed and this is the new name: {}", filepath);
                    break;
                }
            }
        );
    }

    void AssetPanel::OnUpdate([[maybe_unused]] Timestep ts)
    {
        m_ElapsedTime += ts;
        s_LastDomainReloadTime += ts;

        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && GImGui->ActiveId == 0)
        {
            const EditorContext& context = EditorLayer::GetInstance()->GetContext();
            if (context.IsValid(EditorContextType::File))
            {
                const std::filesystem::path path = context.As<char>();
                std::filesystem::remove_all(path);
                EditorLayer::GetInstance()->ResetContext();
            }
        }
    }

    void AssetPanel::OnImGuiRender()
    {
        constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoScrollbar;

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable
            | ImGuiTableFlags_ContextMenuInBody;

        if (OnBegin(windowFlags))
        {
            RenderHeader();
            ImGui::Separator();
            ImVec2 availableRegion = ImGui::GetContentRegionAvail();
            if (ImGui::BeginTable("MainViewTable", 2, tableFlags, availableRegion))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                RenderSideView();
                ImGui::TableNextColumn();
                RenderBody(m_ThumbnailSize >= 96.0f);

                ImGui::EndTable();
            }

            OnEnd();
        }
    }

    void AssetPanel::Invalidate()
    {
        m_AssetsDirectory = CPathMgr::GetInst()->GetContentPath();
        m_CurrentDirectory = m_AssetsDirectory;
        Refresh();
    }

    void AssetPanel::RenderHeader()
    {
        if (ImGui::Button(ICON_MDI_COGS))
        {
            ImGui::OpenPopup("SettingsPopup");
        }
        if (ImGui::BeginPopup("SettingsPopup"))
        {
            if (UI::BeginProperties(ImGuiTableFlags_SizingStretchSame))
            {
                UI::Property("Thumbnail Size", m_ThumbnailSize, 95.9f, 256.0f, nullptr, 0.1f, "");
                UI::EndProperties();
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        const float cursorPosX = ImGui::GetCursorPosX();
        m_Filter.Draw("###ConsoleFilter", ImGui::GetContentRegionAvail().x);
        if (!m_Filter.IsActive())
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(cursorPosX + ImGui::GetFontSize() * 0.5f);
            ImGui::TextUnformatted(ICON_MDI_MAGNIFY " Search...");
        }

        ImGui::Spacing();
        ImGui::Spacing();

        // Back button
        {
            bool disabledBackButton = false;
            if (m_CurrentDirectory == m_AssetsDirectory)
            {
                disabledBackButton = true;
            }

            if (disabledBackButton)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button(ICON_MDI_ARROW_LEFT_CIRCLE_OUTLINE))
            {
                m_BackStack.push(m_CurrentDirectory);
                UpdateDirectoryEntries(m_CurrentDirectory.parent_path());
            }

            if (disabledBackButton)
            {
                ImGui::PopStyleVar();
                ImGui::PopItemFlag();
            }
        }

        ImGui::SameLine();

        // Front button
        {
            bool disabledFrontButton = false;
            if (m_BackStack.empty())
            {
                disabledFrontButton = true;
            }

            if (disabledFrontButton)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button(ICON_MDI_ARROW_RIGHT_CIRCLE_OUTLINE))
            {
                const auto& top = m_BackStack.top();
                UpdateDirectoryEntries(top);
                m_BackStack.pop();
            }

            if (disabledFrontButton)
            {
                ImGui::PopStyleVar();
                ImGui::PopItemFlag();
            }
        }

        ImGui::SameLine();

        ImGui::TextUnformatted(ICON_MDI_FOLDER);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, {0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.0f, 0.0f, 0.0f, 0.0f});
        std::filesystem::path current = m_AssetsDirectory.parent_path();
        std::filesystem::path directoryToOpen;
        std::filesystem::path currentDirectory = m_CurrentDirectory.lexically_relative(current);
        
        for (auto& path : currentDirectory)
        {
            current /= path;
            ImGui::SameLine();
            if (ImGui::Button(path.filename().string().c_str()))
            {
                directoryToOpen = current;
            }

            if (m_CurrentDirectory != current)
            {
                ImGui::SameLine();
                ImGui::TextUnformatted("/");
            }
        }
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();

        if (!directoryToOpen.empty())
        {
            UpdateDirectoryEntries(m_AssetsDirectory / directoryToOpen);
        }
    }

    void AssetPanel::RenderSideView()
    {
        static vector<fs::path> selectionMask;

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg
            | ImGuiTableFlags_NoPadInnerX
            | ImGuiTableFlags_NoPadOuterX
            | ImGuiTableFlags_ContextMenuInBody
            | ImGuiTableFlags_ScrollY;

        constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
            | ImGuiTreeNodeFlags_FramePadding
            | ImGuiTreeNodeFlags_SpanFullWidth;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
        if (ImGui::BeginTable("SideViewTable", 1, tableFlags))
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGuiTreeNodeFlags nodeFlags = treeNodeFlags;
            const bool         selected = m_CurrentDirectory == m_AssetsDirectory && selectionMask.empty();
            if (selected)
            {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
                ImGui::PushStyleColor(ImGuiCol_Header, EditorTheme::HeaderSelectedColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorTheme::HeaderSelectedColor);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorTheme::HeaderHoveredColor);
            }

            bool opened = ImGui::TreeNodeEx(m_AssetsDirectory.string().c_str(), nodeFlags, "");
            ImGui::PopStyleColor(selected ? 2 : 1);

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                UpdateDirectoryEntries(m_AssetsDirectory);
                selectionMask.clear();
            }
            const char* folderIcon = opened ? ICON_MDI_FOLDER_OPEN : ICON_MDI_FOLDER;
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, EditorTheme::AssetIconColor);
            ImGui::TextUnformatted(folderIcon);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::TextUnformatted("Assets");

            if (opened)
            {
                const auto [isClicked, clickedNode] = DirectoryTreeViewRecursive(m_AssetsDirectory,
                    selectionMask, treeNodeFlags);

                if (isClicked)
                {
                    // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                    if (ImGui::GetIO().KeyCtrl)
                    {
                        selectionMask.push_back(clickedNode); // CTRL+click to toggle
                    }
                    else
                    {
                        selectionMask.clear();
                        selectionMask.push_back(clickedNode); // Click to single-select
                    }
                }

                ImGui::TreePop();
            }

            ImGui::EndTable();

            if (ImGui::IsItemClicked())
            {
                EditorLayer::GetInstance()->ResetContext();
            }
        }

        ImGui::PopStyleVar();
    }

    void AssetPanel::RenderBody(bool grid)
    {
        std::filesystem::path directoryToOpen;
        std::filesystem::path directoryToDelete;

        float padding = 4.0f;
        float scaledThumbnailSize = m_ThumbnailSize * ImGui::GetIO().FontGlobalScale;
        float scaledThumbnailSizeX = scaledThumbnailSize * 0.55f;
        float cellSize = scaledThumbnailSizeX + 2 * padding + scaledThumbnailSizeX * 0.1f;

        float overlayPaddingY = 6.0f * padding;
        float thumbnailPadding = overlayPaddingY * 0.5f;
        float thumbnailSize = scaledThumbnailSizeX - thumbnailPadding;

        ImVec2 backgroundThumbnailSize = {scaledThumbnailSizeX + padding * 2, scaledThumbnailSize + padding * 2};

        float panelWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize;
        int   columnCount = static_cast<int>(panelWidth / cellSize);
        if (columnCount < 1)
        {
            columnCount = 1;
        }

        float lineHeight = ImGui::GetTextLineHeight();
        int   flags = ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_ScrollY;

        if (!grid)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
            columnCount = 1;
            flags |= ImGuiTableFlags_RowBg
                | ImGuiTableFlags_NoPadOuterX
                | ImGuiTableFlags_NoPadInnerX
                | ImGuiTableFlags_SizingStretchSame;
        }
        else
        {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {
                scaledThumbnailSizeX * 0.05f, scaledThumbnailSizeX * 0.05f
            });
            flags |= ImGuiTableFlags_PadOuterX | ImGuiTableFlags_SizingFixedFit;
        }

        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImVec2 region = ImGui::GetContentRegionAvail();
        ImGui::InvisibleButton("##DragDropTargetAssetPanelBody", region);

        ImGui::SetItemAllowOverlap();
        ImGui::SetCursorPos(cursorPos);

        bool anyItemHovered = false;
        bool textureCreated = false;
        if (ImGui::BeginTable("BodyTable", columnCount, flags))
        {
            int i = 0;
            for (auto& file : m_DirectoryEntries)
            {
                ImGui::PushID(i);

                bool        isDir = file.IsDirectory;
                const char* filename = file.Name.c_str();

                auto textureId = m_DirectoryIcon->GetSRV().Get();
                if (!isDir)
                {
                    if (file.Type == FileType::Texture)
                    {
                        if (file.Thumbnail)
                        {
                            textureId = file.Thumbnail->GetSRV().Get();
                        }
                        else if (!textureCreated)
                        {
                            textureCreated = true;
                            file.Thumbnail = CResMgr::GetInst()->Load<Texture>(file.Filepath);
                            textureId = file.Thumbnail->GetSRV().Get();
                        }
                        else
                        {
                            textureId = nullptr;
                        }
                    }
                    else
                    {
                        textureId = m_FileIcon->GetSRV().Get();
                    }
                }

                ImGui::TableNextColumn();

                const auto& path = file.DirectoryEntry.path();
                std::string strPath = path.string();

                bool clicked;
                bool opened = false;
                cursorPos = ImGui::GetCursorPos();

                if (grid)
                {
                    bool highlight = false;

                    const EditorContext& context = EditorLayer::GetInstance()->GetContext();
                    if (context.IsValid(EditorContextType::File))
                    {
                        highlight = file.Filepath == context.As<char>();
                    }

                    // Background button
                    static std::string id = "###";
                    id[2] = static_cast<char>(i);
                    clicked = UI::ToggleButton(id.c_str(), highlight, backgroundThumbnailSize, 0.0f, 1.0f);
                }
                else
                {
                    constexpr ImGuiTreeNodeFlags teeNodeFlags = ImGuiTreeNodeFlags_FramePadding
                        | ImGuiTreeNodeFlags_SpanFullWidth
                        | ImGuiTreeNodeFlags_Leaf;

                    opened = ImGui::TreeNodeEx(file.Name.c_str(), teeNodeFlags, "");
                    clicked = ImGui::IsItemClicked();
                }

                {
                    if (m_ElapsedTime > 0.25f && clicked)
                    {
                        EditorLayer::GetInstance()->SetContext(EditorContextType::File, strPath.c_str(),
                            sizeof(char) * (strPath.length() + 1));
                    }
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Delete"))
                        {
                            directoryToDelete = path;
                            ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::MenuItem("Rename"))
                        {
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::Separator();

                        DrawContextMenuItems(path, isDir);
                        ImGui::EndPopup();
                    }
                    ImGui::PopStyleVar();

                    if (isDir)
                    {
                        DragDropTarget(file.Filepath.c_str());
                    }

                    DragDropFrom(file.Filepath);

                    if (ImGui::IsItemHovered())
                    {
                        anyItemHovered = true;
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        if (isDir)
                        {
                            directoryToOpen = path;
                        }
                        else
                        {
                            OpenFile(path);
                        }
                        EditorLayer::GetInstance()->ResetContext();
                    }
                }

                if (grid)
                {
                    // Foreground Image
                    ImGui::SetCursorPos({cursorPos.x + padding, cursorPos.y + padding});
                    ImGui::SetItemAllowOverlap();
                    ImGui::Image(m_WhiteTexture->GetSRV().Get(),
                        {backgroundThumbnailSize.x - padding * 2.0f, backgroundThumbnailSize.y - padding * 2.0f},
                        {0, 0}, {1, 1}, EditorTheme::WindowBgAlternativeColor);

                    // Thumbnail Image
                    ImGui::SetCursorPos({cursorPos.x + thumbnailPadding * 0.75f, cursorPos.y + thumbnailPadding});
                    ImGui::SetItemAllowOverlap();
                    ImGui::Image(textureId, {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

                    // Type Color frame
                    ImVec2 typeColorFrameSize = {scaledThumbnailSizeX, scaledThumbnailSizeX * 0.03f};
                    ImGui::SetCursorPosX(cursorPos.x + padding);
                    ImGui::Image(m_WhiteTexture->GetSRV().Get(), typeColorFrameSize, {0, 0}, {1, 1},
                        isDir ? ImVec4(0.0f, 0.0f, 0.0f, 0.0f) : file.FileTypeIndicatorColor);

                    ImVec2 rectMin = ImGui::GetItemRectMin();
                    ImVec2 rectSize = ImGui::GetItemRectSize();
                    ImRect clipRect = ImRect({rectMin.x + padding * 1.0f, rectMin.y + padding * 2.0f},
                        {
                            rectMin.x + rectSize.x,
                            rectMin.y + scaledThumbnailSizeX - EditorTheme::SmallFont->FontSize - padding * 4.0f
                        });
                    UI::ClippedText(clipRect.Min, clipRect.Max, filename, nullptr, nullptr, {0, 0}, nullptr,
                        clipRect.GetSize().x);

                    if (!isDir)
                    {
                        ImGui::SetCursorPos({
                            cursorPos.x + padding * 2.0f,
                            cursorPos.y + backgroundThumbnailSize.y - EditorTheme::SmallFont->FontSize - padding * 2.0f
                        });
                        ImGui::BeginDisabled();
                        ImGui::PushFont(EditorTheme::SmallFont);
                        ImGui::TextUnformatted(file.FileTypeString.data());
                        ImGui::PopFont();
                        ImGui::EndDisabled();
                    }
                }
                else
                {
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - lineHeight);
                    ImGui::Image(textureId, {lineHeight, lineHeight}, {0, 1}, {1, 0});
                    ImGui::SameLine();
                    ImGui::TextUnformatted(filename);

                    if (opened)
                    {
                        ImGui::TreePop();
                    }
                }


                ImGui::PopID();
                ++i;
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, EditorTheme::PopupItemSpacing);
            if (ImGui::BeginPopupContextWindow("AssetPanelHierarchyContextWindow",
                ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                EditorLayer::GetInstance()->ResetContext();
                DrawContextMenuItems(m_CurrentDirectory, true);
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::EndTable();

            if (!anyItemHovered && ImGui::IsItemClicked())
            {
                EditorLayer::GetInstance()->ResetContext();
            }
        }

        ImGui::PopStyleVar();

        if (!directoryToDelete.empty())
        {
            std::filesystem::remove_all(directoryToDelete);
            EditorLayer::GetInstance()->ResetContext();
        }

        if (!directoryToOpen.empty())
        {
            UpdateDirectoryEntries(directoryToOpen);
        }
    }

    void AssetPanel::UpdateDirectoryEntries(const std::filesystem::path& directory)
    {
        m_CurrentDirectory = directory;
        m_DirectoryEntries.clear();
        auto directoryIt = std::filesystem::directory_iterator(directory);
        for (auto& directoryEntry : directoryIt)
        {
            const auto& path = directoryEntry.path();
            auto        relativePath = path.lexically_relative(m_AssetsDirectory);

            std::string filename = relativePath.filename().string();
            std::string extension = relativePath.extension().string();

            auto        fileType = FileType::Unknown;
            const auto& fileTypeIt = s_FileTypes.find(extension);
            if (fileTypeIt != s_FileTypes.end())
            {
                fileType = fileTypeIt->second;
            }

            std::string_view fileTypeString = s_FileTypesToString.at(FileType::Unknown);
            const auto&      fileStringTypeIt = s_FileTypesToString.find(fileType);
            if (fileStringTypeIt != s_FileTypesToString.end())
            {
                fileTypeString = fileStringTypeIt->second;
            }

            ImVec4      fileTypeColor = {1.0f, 1.0f, 1.0f, 1.0f};
            const auto& fileTypeColorIt = s_TypeColors.find(fileType);
            if (fileTypeColorIt != s_TypeColors.end())
            {
                fileTypeColor = fileTypeColorIt->second;
            }

            m_DirectoryEntries.push_back({
                filename, path.string(), extension, directoryEntry, nullptr, directoryEntry.is_directory(),
                fileType, fileTypeString, fileTypeColor
            });
        }

        m_ElapsedTime = 0.0f;
    }

    void AssetPanel::DrawContextMenuItems(const std::filesystem::path& context, bool isDir)
    {
        if (isDir)
        {
            if (ImGui::BeginMenu("Create"))
            {
                if (ImGui::MenuItem("Folder"))
                {
                    int         i = 0;
                    bool        created = false;
                    std::string newFolderPath;
                    while (!created)
                    {
                        std::string folderName = "New Folder" + (i == 0 ? "" : fmt::format(" ({})", i));
                        newFolderPath = (context / folderName).string();
                        created = std::filesystem::create_directory(newFolderPath);
                        ++i;
                    }
                    EditorLayer::GetInstance()->SetContext(EditorContextType::File, newFolderPath.c_str(),
                        sizeof(char) * (newFolderPath.length() + 1));
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndMenu();
            }
        }
        if (ImGui::MenuItem("Show in Explorer"))
        {
            // FileDialogs::OpenFolderAndSelectItem(context.string().c_str());
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Open"))
        {
            // FileDialogs::OpenFileWithProgram(context.string().c_str());
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Copy Path"))
        {
            ImGui::SetClipboardText(context.string().c_str());
            ImGui::CloseCurrentPopup();
        }

        if (isDir)
        {
            if (ImGui::MenuItem("Refresh"))
            {
                Refresh();
                ImGui::CloseCurrentPopup();
            }
        }
    }
}
