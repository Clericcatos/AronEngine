#include "framework.h"
#include "EngineApp.h"
#include "EngineTime.h"
#include "Input.h"
#include "Renderer.h"
#include "../Managers/SceneManager.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/SimpleAudioManager.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/ScriptSystem.h"
#include "../Editor/EditorSystem.h"
#include "../Editor/ImGuiEditorSystem.h"

namespace AronEngine
{
    EngineApp::EngineApp()
        : hInstance(nullptr)
        , hWnd(nullptr)
        , width(1920)
        , height(1080)
        , windowTitle(L"AronEngine")
        , windowClassName(L"AronEngineWindow")
        , isRunning(false)
        , isEditorMode(true)
    {
    }

    EngineApp::~EngineApp()
    {
    }

    bool EngineApp::Initialize(HINSTANCE hInstance, int width, int height, const std::wstring& title, bool editorMode)
    {
        this->hInstance = hInstance;
        this->width = width;
        this->height = height;
        this->windowTitle = title;
        this->isEditorMode = editorMode;

        if (!CreateAppWindow())
        {
            DEBUG_LOG("Failed to create window");
            return false;
        }

        time = std::make_unique<Time>();
        input = std::make_unique<Input>();
        renderer = std::make_unique<Renderer>();
        sceneManager = std::make_unique<SceneManager>();
        resourceManager = std::make_unique<ResourceManager>();
        renderSystem = std::make_unique<RenderSystem>();
        transformSystem = std::make_unique<TransformSystem>();
        scriptSystem = std::make_unique<ScriptSystem>();

        if (!renderer->Initialize(hWnd, width, height))
        {
            DEBUG_LOG("Failed to initialize renderer");
            return false;
        }

        if (!input->Initialize(hWnd))
        {
            DEBUG_LOG("Failed to initialize input");
            return false;
        }

        if (!SimpleAudioManager::GetInstance().Initialize())
        {
            DEBUG_LOG("Failed to initialize simple audio manager");
            return false;
        }

        if (isEditorMode)
        {
            editorSystem = std::make_unique<EditorSystem>();
            if (!editorSystem->Initialize(hWnd, renderer.get()))
            {
                DEBUG_LOG("Failed to initialize editor system");
                return false;
            }
        }

        OnInit();

        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);

        isRunning = true;
        return true;
    }

    void EngineApp::Run()
    {
        MSG msg = { 0 };

        time->Start();

        while (isRunning)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    isRunning = false;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                time->Update();
                float deltaTime = time->GetDeltaTime();

                Update(deltaTime);
                Render();
            }
        }
    }

    void EngineApp::Shutdown()
    {
        OnShutdown();

        if (editorSystem)
        {
            editorSystem->Shutdown();
        }

        scriptSystem.reset();
        transformSystem.reset();
        renderSystem.reset();
        resourceManager.reset();
        sceneManager.reset();
        renderer.reset();
        input.reset();
        time.reset();
        editorSystem.reset();
        
        SimpleAudioManager::GetInstance().Shutdown();

        if (hWnd)
        {
            DestroyWindow(hWnd);
            hWnd = nullptr;
        }

        UnregisterClass(windowClassName.c_str(), hInstance);
    }

    bool EngineApp::CreateAppWindow()
    {
        WNDCLASSEX wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = windowClassName.c_str();
        wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        if (!RegisterClassEx(&wcex))
        {
            return false;
        }

        RECT rc = { 0, 0, width, height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        hWnd = CreateWindow(
            windowClassName.c_str(),
            windowTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rc.right - rc.left,
            rc.bottom - rc.top,
            nullptr,
            nullptr,
            hInstance,
            this
        );

        if (!hWnd)
        {
            return false;
        }

        return true;
    }

    void EngineApp::Update(float deltaTime)
    {
        input->Update();

        if (editorSystem && isEditorMode)
        {
            editorSystem->Update(deltaTime);
        }

        if (sceneManager->GetActiveScene())
        {
            transformSystem->Update(deltaTime);
            scriptSystem->Update(deltaTime);
        }

        OnUpdate(deltaTime);
    }

    void EngineApp::Render()
    {
        renderer->BeginDraw();
        renderer->Clear(Color(0.2f, 0.2f, 0.2f));

        if (sceneManager->GetActiveScene())
        {
            renderSystem->Render(renderer.get());
        }

        OnRender();

        if (editorSystem && isEditorMode)
        {
            editorSystem->Render();
        }

        renderer->EndDraw();
    }

    void EngineApp::ProcessMessages()
    {
        MSG msg = { 0 };
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT CALLBACK EngineApp::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        EngineApp* app = nullptr;

        if (message == WM_CREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            app = (EngineApp*)pCreate->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)app);
        }
        else
        {
            app = (EngineApp*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }

        if (app)
        {
            // Handle ImGui messages first
            auto& imguiEditor = ImGuiEditorSystem::GetInstance();
            if (imguiEditor.ProcessWndProc(hWnd, message, wParam, lParam))
            {
                return 0;
            }
            
            if (app->editorSystem && app->isEditorMode)
            {
                if (app->editorSystem->ProcessWndProc(hWnd, message, wParam, lParam))
                {
                    return 0;
                }
            }

            switch (message)
            {
            case WM_SIZE:
                if (app->renderer && wParam != SIZE_MINIMIZED)
                {
                    app->width = LOWORD(lParam);
                    app->height = HIWORD(lParam);
                    app->renderer->Resize(app->width, app->height);
                }
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEMOVE:
            case WM_MOUSEWHEEL:
                if (app->input)
                {
                    app->input->ProcessMessage(message, wParam, lParam);
                }
                return 0;
            }
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}