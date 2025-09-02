#include "framework.h"
#include "EngineApp.h"
#include "EngineTime.h"
#include "Input.h"
#include "Renderer.h"
#include "../Managers/SceneManager.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/AudioManager.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/ScriptSystem.h"
#include "../Editor/LightUnityEditor.h"
#include <shellapi.h>

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
        OutputDebugStringA("[EngineApp] Initialize started\n");
        this->hInstance = hInstance;
        this->width = width;
        this->height = height;
        this->windowTitle = title;
        this->isEditorMode = editorMode;

        OutputDebugStringA("[EngineApp] Creating window...\n");
        if (!CreateAppWindow())
        {
            DEBUG_LOG("Failed to create window");
            OutputDebugStringA("[EngineApp] ERROR: Failed to create window\n");
            return false;
        }
        OutputDebugStringA("[EngineApp] Window created successfully\n");

        OutputDebugStringA("[EngineApp] Creating engine components...\n");
        time = std::make_unique<Time>();
        input = std::make_unique<Input>();
        renderer = std::make_unique<Renderer>();
        sceneManager = std::make_unique<SceneManager>();
        resourceManager = std::make_unique<ResourceManager>();
        renderSystem = std::make_unique<RenderSystem>();
        transformSystem = std::make_unique<TransformSystem>();
        scriptSystem = std::make_unique<ScriptSystem>();
        OutputDebugStringA("[EngineApp] Engine components created\n");

        OutputDebugStringA("[EngineApp] Initializing renderer...\n");
        if (!renderer->Initialize(hWnd, width, height))
        {
            DEBUG_LOG("Failed to initialize renderer");
            OutputDebugStringA("[EngineApp] ERROR: Failed to initialize renderer\n");
            return false;
        }
        OutputDebugStringA("[EngineApp] Renderer initialized successfully\n");

        OutputDebugStringA("[EngineApp] Initializing input...\n");
        if (!input->Initialize(hWnd))
        {
            DEBUG_LOG("Failed to initialize input");
            OutputDebugStringA("[EngineApp] ERROR: Failed to initialize input\n");
            return false;
        }
        OutputDebugStringA("[EngineApp] Input initialized successfully\n");

        OutputDebugStringA("[EngineApp] Initializing audio manager...\n");
        if (!AudioManager::GetInstance().Initialize(hWnd))
        {
            DEBUG_LOG("Failed to initialize audio manager");
            OutputDebugStringA("[EngineApp] WARNING: Failed to initialize audio manager (continuing anyway)\n");
            // Don't return false - audio is optional
        }
        else
        {
            OutputDebugStringA("[EngineApp] Audio manager initialized successfully\n");
        }

        OutputDebugStringA("[EngineApp] Calling OnInit...\n");
        OnInit();
        OutputDebugStringA("[EngineApp] OnInit completed\n");

        OutputDebugStringA("[EngineApp] Showing window...\n");
        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);

        isRunning = true;
        OutputDebugStringA("[EngineApp] Initialize completed successfully\n");
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


        scriptSystem.reset();
        transformSystem.reset();
        renderSystem.reset();
        resourceManager.reset();
        sceneManager.reset();
        renderer.reset();
        input.reset();
        time.reset();
        
        AudioManager::GetInstance().Shutdown();

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
        
        // Enable drag and drop
        DragAcceptFiles(hWnd, TRUE);

        return true;
    }

    void EngineApp::Update(float deltaTime)
    {
        input->Update();
        
        // Update FMOD system
        AudioManager::GetInstance().Update();


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

            switch (message)
            {
            case WM_DROPFILES:
                {
                    HDROP hDrop = (HDROP)wParam;
                    UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
                    
                    for (UINT i = 0; i < fileCount; i++)
                    {
                        wchar_t filePath[MAX_PATH];
                        if (DragQueryFileW(hDrop, i, filePath, MAX_PATH) > 0)
                        {
                            // Only handle drag drop if in editor mode and editor is available
                            if (app->isEditorMode)
                            {
                                // Check if LightUnityEditor is enabled before using it
                                if (LightUnityEditor::GetInstance().IsEnabled())
                                {
                                    LightUnityEditor::GetInstance().HandleDragDrop(filePath);
                                }
                            }
                        }
                    }
                    
                    DragFinish(hDrop);
                }
                return 0;
                
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