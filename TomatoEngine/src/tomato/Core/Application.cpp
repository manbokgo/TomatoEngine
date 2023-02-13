#include "pch.h"
#include "tomato/Core/Application.h"

#include "CKeyMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "tomato/Renderer/Device.h"
#include "tomato/Renderer/Renderer2D.h"
#include "tomato/Renderer/Renderer3D.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace tomato
{
    Application* Application::s_Instance = nullptr;

    Application::Application(wstring_view name, HINSTANCE hInstance)
    {
        ASSERT(!s_Instance);
        s_Instance = this;

#ifdef _DEBUG
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        //_CrtSetBreakAlloc(694);
#endif

        MyRegisterClass(hInstance);
        if (!InitInstance(hInstance, name))
        {
            ASSERT(false);
        }

        ShowWindow(m_WindowHandle, SW_SHOWMAXIMIZED);

        RECT rt;
        GetClientRect(m_WindowHandle, &rt);
        m_WindowSize.x = (float)(rt.right - rt.left);
        m_WindowSize.y = (float)(rt.bottom - rt.top);


        // Manager ÃÊ±âÈ­
        if (FAILED(Device::GetInst()->init(m_WindowHandle, m_WindowSize)))
        {
            ASSERT(false);
        }

        Renderer2D::Init();
        Renderer3D::GetInst()->Init();
        CPathMgr::GetInst()->Init();
        CKeyMgr::GetInst()->init();
        CTimeMgr::GetInst()->init();

        CResMgr::GetInst()->Init();




        /*
        m_Window = Window::Create(WindowProps(name));
        m_Window->SetEventCallBack(TOMATO_BIND_EVENT_FN(Application::OnEvent));

        Renderer::Init();
        AudioEngine::Init();
        ScriptEngine::Init();
        */

        m_LayerStack = new LayerStack();
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        delete m_LayerStack;

        Renderer2D::Shutdown();
        Renderer3D::GetInst()->Shutdown();
        /*
        ScriptEngine::Shutdown();
        AudioEngine::Shutdown();
        Renderer::Shutdown();
        */

    }

    void Application::PushLayer(Layer* layer) const
    {
        m_LayerStack->PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer) const
    {
        m_LayerStack->PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    size_t Application::GetAllocatedMemorySize()
    {
        return Allocation::GetSize();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(TOMATO_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(TOMATO_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); ++it)
        {
            if (e.Handled)
            {
                break;
            }

            (*it)->OnEvent(e);
        }
    }

    void Application::Run()
    {
        MSG msg;
        while (m_Running)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);

                continue;
            }
            
            CTimeMgr::GetInst()->tick();
            Timestep timestep = CTimeMgr::GetInst()->GetDeltaTime();

            ExecuteMainThreadQueue();

            if (!m_Minimized)
            {
                for (Layer* layer : *m_LayerStack)
                {
                    layer->OnUpdate(timestep);
                }

                Device::GetInst()->TargetSet();
                Device::GetInst()->TargetClear();


                m_ImGuiLayer->Begin();
                {
                    for (Layer* layer : *m_LayerStack)
                    {
                        layer->OnImGuiRender();
                    }
                }
                m_ImGuiLayer->End();

                Device::GetInst()->Present();
            }
        }
    }

    void Application::SubmitToMainThread(const std::function<void()>& function)
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        m_MainThreadQueue.emplace_back(function);
    }

    bool Application::OnWindowClose([[maybe_unused]] const WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(const WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;

        /*
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        */

        return false;
    }

    void Application::ExecuteMainThreadQueue()
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        for (const auto& func : m_MainThreadQueue)
        {
            func();
        }

        m_MainThreadQueue.clear();
    }



    ATOM Application::MyRegisterClass(HINSTANCE hInstance)
    {
        WNDCLASSEXW wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Application::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
        wcex.lpszMenuName = nullptr; //MAKEINTRESOURCEW(IDC_CLIENT);
        wcex.lpszClassName = L"WindowClass";
        wcex.hIconSm = nullptr;

        return RegisterClassExW(&wcex);
    }

    BOOL Application::InitInstance(HINSTANCE hInstance, wstring_view name)
    {
        m_WindowHandle = CreateWindowW(L"WindowClass",
            name.data(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

        if (!m_WindowHandle)
        {
            return FALSE;
        }

        ShowWindow(m_WindowHandle, false);
        UpdateWindow(m_WindowHandle);

        return TRUE;
    }


    LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        {
            return true;
        }

        switch (message)
        {
            // case WM_DPICHANGED:
            //     if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            //     {
            //         //const int dpi = HIWORD(wParam);
            //         //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            //         const RECT* suggested_rect = (RECT*)lParam;
            //         SetWindowPos(hWnd,
            //             nullptr,
            //             suggested_rect->left,
            //             suggested_rect->top,
            //             suggested_rect->right - suggested_rect->left,
            //             suggested_rect->bottom - suggested_rect->top,
            //             SWP_NOZORDER | SWP_NOACTIVATE);
            //     }
            //     break;
        case WM_SIZE:
        {
            const UINT width = LOWORD(lParam);
            const UINT height = HIWORD(lParam);
            Device::GetInst()->OnWindowResize(width, height);
        
        }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

}
