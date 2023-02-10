#pragma once

#include "tomato/Core/Timestep.h"
#include "tomato/Events/Event.h"

namespace tomato
{
	class Layer
	{
	public:
		explicit Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() { /* Called when layer is attached */ }
		virtual void OnDetach() { /* Called when layer is detached */ }
		virtual void OnUpdate([[maybe_unused]] Timestep ts) { /* Layer OnUpdate */ }
		virtual void OnImGuiRender() { /* Layer OnRender */ }
		virtual void OnEvent([[maybe_unused]] Event& e) { /* Called when an event is fired */ }

		[[nodiscard]] const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
