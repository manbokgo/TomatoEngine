#include "pch.h"
#include <tomato/Core/Application.h>
#include <tomato/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace tomato
{
    class TomatoEditor final : public Application
    {
    public:
        TomatoEditor(HINSTANCE hInstance)
            : Application(L"Tomato Editor", hInstance)
        {
            PushLayer(new EditorLayer());
        }

        ~TomatoEditor() override = default; 
    };

    Application* CreateApplication(HINSTANCE hInstance)
    {
        return new TomatoEditor(hInstance);
    }
}
