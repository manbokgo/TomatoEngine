#pragma once

#include "tomato/Core/Timestep.h"

namespace tomato
{
	class BasePanel
	{
	public:
		bool Showing;

	public:
		BasePanel(const char* name = "Unnamed Panel", const char* icon = u8"", bool defaultShow = false);
		virtual ~BasePanel() = default;

		BasePanel(const BasePanel& other) = delete;
		BasePanel(BasePanel&& other) = delete;
		BasePanel& operator=(const BasePanel& other) = delete;
		BasePanel& operator=(BasePanel&& other) = delete;

		virtual void OnUpdate([[maybe_unused]] Timestep ts) { /* Not pure virtual */ }
		virtual void OnImGuiRender() = 0;
		
		[[nodiscard]] const char* GetName() const { return m_Name.c_str(); }
		[[nodiscard]] const char* GetIcon() const { return m_Icon; }

	protected:
		bool OnBegin(int32_t windowFlags = 0);
		void OnEnd() const;

	protected:
		std::string m_Name;
		const char* m_Icon;
		std::string m_ID;

	private:
		static uint32_t s_Count;
	};
}
