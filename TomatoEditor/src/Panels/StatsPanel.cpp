#include "pch.h"
#include "StatsPanel.h"

#include <icons/IconsMaterialDesignIcons.h>

#include "../Utils/UI.h"
#include "tomato/Renderer/Device.h"
#include "tomato/Renderer/Renderer2D.h"

namespace tomato
{
	StatsPanel::StatsPanel(const char* name)
		: BasePanel(name, ICON_MDI_INFORMATION_VARIANT, true), m_FpsValues{}
	{
	}

	void StatsPanel::OnImGuiRender()
	{
		float avg = 0.0;

		const size_t size = m_FrameTimes.size();
		if (size >= 120)
        {
            m_FrameTimes.erase(m_FrameTimes.begin());
        }

        m_FrameTimes.emplace_back(ImGui::GetIO().Framerate);
		for (uint32_t i = 0; i < size; i++)
		{
			float frameTime = m_FrameTimes[i];
			m_FpsValues[i] = frameTime;
			avg += frameTime;
		}
		
		avg /= static_cast<float>(size);

		if (OnBegin())
		{
		    const auto stats = Renderer2D::GetStats();
		    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			
			if (UI::BeginProperties())
			{
				bool vSync = Device::GetInst()->IsVSync();
				if (UI::Property("VSync Enabled", vSync))
				{
					Device::GetInst()->SetVSync(vSync);
				}

				UI::EndProperties();
			}

			ImGui::PlotLines("##FPS", m_FpsValues, static_cast<int>(size));
			ImGui::Text("FPS: %lf", static_cast<double>(avg));
			const double fps = (1.0 / static_cast<double>(avg)) * 1000.0;
			ImGui::Text("Frame time (ms): %lf", fps);

			OnEnd();
		}
	}
}
