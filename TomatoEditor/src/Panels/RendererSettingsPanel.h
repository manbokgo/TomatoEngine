#pragma once

#include "BasePanel.h"

namespace tomato
{
	class RendererSettingsPanel : public BasePanel
	{
	public:
		explicit RendererSettingsPanel(const char* name = "Render Settings");
		~RendererSettingsPanel() override = default;

		RendererSettingsPanel(const RendererSettingsPanel& other) = delete;
		RendererSettingsPanel(RendererSettingsPanel&& other) = delete;
		RendererSettingsPanel& operator=(const RendererSettingsPanel& other) = delete;
		RendererSettingsPanel& operator=(RendererSettingsPanel&& other) = delete;

		void OnImGuiRender() override;
	};
}
