#pragma once

#include "tomato/Core/Layer.h"

namespace tomato
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent([[maybe_unused]] Event& e) override;

		void Begin() const;
		void End() const;

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
	};
}
