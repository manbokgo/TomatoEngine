#pragma once

#include <imgui/imgui.h>
#include <stack>

#include "BasePanel.h"
#include "tomato/Core/Timestep.h"

namespace tomato
{
    enum class FileType
    {
        Unknown = 0,
        Scene,
        Prefab,
        Shader,
        Texture,
        Cubemap,
        Model,
        Audio
    };

	class AssetPanel : public BasePanel
	{
	public:
		explicit AssetPanel(const char* name = "Assets");

		~AssetPanel() override = default;

		AssetPanel(const AssetPanel& other) = delete;
		AssetPanel(AssetPanel&& other) = delete;
		AssetPanel& operator=(const AssetPanel& other) = delete;
		AssetPanel& operator=(AssetPanel&& other) = delete;

		void OnUpdate([[maybe_unused]] Timestep ts) override;
		void OnImGuiRender() override;

		void Invalidate();

	private:
        std::pair<bool, fs::path> DirectoryTreeViewRecursive(const std::filesystem::path& path,
                                                             vector<fs::path>& selectionMask, ImGuiTreeNodeFlags flags);
		void                      RenderHeader();
		void                      RenderSideView();
		void                      RenderBody(bool grid);
		void                      UpdateDirectoryEntries(const std::filesystem::path& directory);
		void                      Refresh() { UpdateDirectoryEntries(m_CurrentDirectory); }

		void DrawContextMenuItems(const std::filesystem::path& context, bool isDir);

	private:
		struct File
		{
			std::string Name;
			std::string Filepath;
			std::string Extension;
			std::filesystem::directory_entry DirectoryEntry;
			Ref<Texture> Thumbnail = nullptr;
			bool IsDirectory = false;

			FileType Type;
			std::string_view FileTypeString;
			ImVec4 FileTypeIndicatorColor;
		};

		std::filesystem::path m_AssetsDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::stack<std::filesystem::path> m_BackStack;
		std::vector<File> m_DirectoryEntries;
		uint32_t m_CurrentlyVisibleItemsTreeView = 0;
		float m_ThumbnailSize = 128.0f;
		ImGuiTextFilter m_Filter;
		float m_ElapsedTime = 0.0f;

		Ref<Texture> m_WhiteTexture;
		Ref<Texture> m_DirectoryIcon;
		Ref<Texture> m_FileIcon;
	};
}
