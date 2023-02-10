#pragma once

#include <mutex>

#include "tomato/Core/LayerStack.h"
#include "tomato/Events/ApplicationEvent.h"

#include "tomato/ImGui/ImGuiLayer.h"

namespace tomato
{
    class Application
    {
        friend int APIENTRY::wWinMain(_In_ HINSTANCE     hInstance,
                                      _In_opt_ HINSTANCE hPrevInstance,
                                      _In_ LPWSTR        lpCmdLine,
                                      _In_ int           nCmdShow);

    public:
        explicit Application(wstring_view name, HINSTANCE hInstance);
        virtual  ~Application();

        Application(const Application& other) = delete;
        Application(Application&& other) = delete;

        void OnEvent(Event& e);

        void PushLayer(Layer* layer) const;
        void PushOverlay(Layer* layer) const;

        [[nodiscard]] HWND GetWindowHandle() const { return m_WindowHandle; }

        void Close();

        [[nodiscard]] ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

        [[nodiscard]] static size_t       GetAllocatedMemorySize();
        [[nodiscard]] static Application& Get() { return *s_Instance; }

        void SubmitToMainThread(const std::function<void()>& function);

    private:
        void               Run();
        [[nodiscard]] bool OnWindowClose([[maybe_unused]] const WindowCloseEvent& e);
        [[nodiscard]] bool OnWindowResize(const WindowResizeEvent& e);
        void               ExecuteMainThreadQueue();
        ATOM               MyRegisterClass(HINSTANCE hInstance);
        BOOL               InitInstance(HINSTANCE hInstance, wstring_view name);
        static LRESULT     WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        HWND        m_WindowHandle;
        ImGuiLayer* m_ImGuiLayer;
        bool        m_Running = true;
        bool        m_Minimized = false;
        LayerStack* m_LayerStack;
        float       m_LastFrameTime = 0.0f;

        std::vector<std::function<void()>> m_MainThreadQueue;
        std::mutex                         m_MainThreadQueueMutex;

        Vec2 m_WindowSize;

    private:
        static Application* s_Instance;
    };

    // 클라이언트에서 정의 필요
    Application* CreateApplication(HINSTANCE hInstance);
}
