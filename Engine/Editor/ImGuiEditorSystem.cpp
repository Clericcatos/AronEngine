#include "framework.h"
#include "ImGuiEditorSystem.h"
#include "../Core/Renderer.h"
#include "../Core/Scene.h"
#include "../Core/GameObject.h"
#include "../Core/TextureManager.h"
#include "../Components/Transform.h"
#include "../Components/SpriteRenderer.h"
#include "KoreanStrings.h"

// ImGui includes  
#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/imgui_internal.h"
#include "../../ThirdParty/ImGui/backends/imgui_impl_win32.h"
#include "../../ThirdParty/ImGui/backends/imgui_impl_dx11.h"

#include <d3d11.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace AronEngine
{
    ImGuiEditorSystem::ImGuiEditorSystem()
        : hWnd(nullptr)
        , renderer(nullptr)
        , isEnabled(true)
        , showDemoWindow(true)
        , showSceneHierarchy(true)
        , showInspector(true)
        , showConsole(true)
        , showAssetBrowser(true)
        , showGameView(false)
        , showSceneView(true)
        , imguiContext(nullptr)
        , d3d11Device(nullptr)
        , d3d11Context(nullptr)
        , selectedObject(nullptr)
        , currentScene(nullptr)
        , isDockingEnabled(true)
        , isViewportsEnabled(false)
    {
    }

    ImGuiEditorSystem::~ImGuiEditorSystem()
    {
        Shutdown();
    }

    bool ImGuiEditorSystem::Initialize(HWND hWnd, Renderer* renderer)
    {
        this->hWnd = hWnd;
        this->renderer = renderer;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(imguiContext);
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        
        if (isDockingEnabled)
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        
        if (isViewportsEnabled)
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        SetupImGuiStyle();

        // Setup Platform/Renderer backends
        if (!ImGui_ImplWin32_Init(hWnd))
        {
            DEBUG_LOG("ImGui Win32 backend initialization failed");
            return false;
        }

        // Create a D3D11 device for ImGui rendering
        D3D_FEATURE_LEVEL featureLevel;
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_DEBUG,
            nullptr, 0,
            D3D11_SDK_VERSION,
            &d3d11Device,
            &featureLevel,
            &d3d11Context
        );

        if (SUCCEEDED(hr))
        {
            if (!ImGui_ImplDX11_Init(d3d11Device, d3d11Context))
            {
                DEBUG_LOG("ImGui DX11 backend initialization failed");
                d3d11Device->Release();
                d3d11Context->Release();
                d3d11Device = nullptr;
                d3d11Context = nullptr;
                return false;
            }
        }
        else
        {
            DEBUG_LOG("Failed to create D3D11 device for ImGui");
            return false;
        }

        DEBUG_LOG("ImGui Editor System initialized successfully");
        DEBUG_LOG("Docking enabled: " + std::string(isDockingEnabled ? "Yes" : "No"));
        DEBUG_LOG("Viewports enabled: " + std::string(isViewportsEnabled ? "Yes" : "No"));

        return true;
    }

    void ImGuiEditorSystem::Shutdown()
    {
        if (imguiContext)
        {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext(imguiContext);
            imguiContext = nullptr;
            
            if (d3d11Context)
            {
                d3d11Context->Release();
                d3d11Context = nullptr;
            }
            
            if (d3d11Device)
            {
                d3d11Device->Release();
                d3d11Device = nullptr;
            }
            
            DEBUG_LOG("ImGui Editor System shutdown");
        }
    }

    void ImGuiEditorSystem::BeginFrame()
    {
        if (!isEnabled || !imguiContext) return;

        ImGui::SetCurrentContext(imguiContext);
        
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (isDockingEnabled)
        {
            SetupDocking();
        }
    }

    void ImGuiEditorSystem::EndFrame()
    {
        if (!isEnabled || !imguiContext) return;

        ImGui::SetCurrentContext(imguiContext);
        
        // Rendering
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiEditorSystem::Update(float deltaTime)
    {
        if (!isEnabled) return;
        // Editor logic updates can go here
    }

    void ImGuiEditorSystem::Render()
    {
        if (!isEnabled || !imguiContext) return;

        ImGui::SetCurrentContext(imguiContext);

        // Main menu bar
        RenderMainMenuBar();

        // Show demo window if requested
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        // Editor windows
        if (showSceneHierarchy)
            RenderSceneHierarchy();

        if (showInspector)
            RenderInspector();

        if (showConsole)
            RenderConsole();

        if (showAssetBrowser)
            RenderAssetBrowser();

        if (showSceneView)
            RenderSceneView();

        if (showGameView)
            RenderGameView();
    }

    void ImGuiEditorSystem::RenderMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(Korean::MENU_FILE))
            {
                if (ImGui::MenuItem(Korean::FILE_NEW_SCENE, "Ctrl+N")) 
                {
                    NewScene();
                }
                if (ImGui::MenuItem(Korean::FILE_OPEN_SCENE, "Ctrl+O")) 
                {
                    LoadScene();
                }
                if (ImGui::MenuItem(Korean::FILE_SAVE_SCENE, "Ctrl+S")) 
                {
                    SaveScene();
                }
                ImGui::Separator();
                if (ImGui::MenuItem(Korean::FILE_EXIT, "Alt+F4")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(Korean::MENU_EDIT))
            {
                if (ImGui::MenuItem(Korean::EDIT_UNDO, "Ctrl+Z")) {}
                if (ImGui::MenuItem(Korean::EDIT_REDO, "Ctrl+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem(Korean::EDIT_CUT, "Ctrl+X")) {}
                if (ImGui::MenuItem(Korean::EDIT_COPY, "Ctrl+C")) {}
                if (ImGui::MenuItem(Korean::EDIT_PASTE, "Ctrl+V")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(Korean::MENU_GAMEOBJECT))
            {
                if (ImGui::MenuItem(Korean::GO_CREATE_EMPTY)) 
                {
                    CreateGameObject("GameObject");
                }
                ImGui::Separator();
                if (ImGui::BeginMenu(Korean::GO_2D_OBJECT))
                {
                    if (ImGui::MenuItem(Korean::GO_SPRITE)) 
                    {
                        CreateSpriteObject("Sprite");
                    }
                    if (ImGui::MenuItem(Korean::GO_SQUARE)) 
                    {
                        CreateSquareObject("Square");
                    }
                    if (ImGui::MenuItem(Korean::GO_CIRCLE)) 
                    {
                        CreateCircleObject("Circle");
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(Korean::MENU_WINDOW))
            {
                ImGui::MenuItem(Korean::WIN_SCENE_HIERARCHY, nullptr, &showSceneHierarchy);
                ImGui::MenuItem(Korean::WIN_INSPECTOR, nullptr, &showInspector);
                ImGui::MenuItem(Korean::WIN_CONSOLE, nullptr, &showConsole);
                ImGui::MenuItem(Korean::WIN_ASSET_BROWSER, nullptr, &showAssetBrowser);
                ImGui::MenuItem(Korean::WIN_SCENE_VIEW, nullptr, &showSceneView);
                ImGui::MenuItem(Korean::WIN_GAME_VIEW, nullptr, &showGameView);
                ImGui::Separator();
                ImGui::MenuItem(Korean::WIN_DEMO, nullptr, &showDemoWindow);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(Korean::MENU_HELP))
            {
                if (ImGui::MenuItem("About AronEngine")) {}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void ImGuiEditorSystem::RenderSceneHierarchy()
    {
        if (ImGui::Begin("Scene Hierarchy", &showSceneHierarchy))
        {
            if (currentScene)
            {
                ImGui::Text("Scene: %s", currentScene->GetName().c_str());
                ImGui::Separator();

                // Render actual game objects
                ImGui::Text("Game Objects:");
                
                for (auto& gameObject : gameObjects)
                {
                    bool isSelected = (selectedObject == gameObject.get());
                    if (ImGui::Selectable(gameObject->GetName().c_str(), isSelected))
                    {
                        selectedObject = gameObject.get();
                    }
                    
                    // Right-click context menu for each object
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Delete"))
                        {
                            // Remove from vector
                            gameObjects.erase(
                                std::remove_if(gameObjects.begin(), gameObjects.end(),
                                    [gameObject = gameObject.get()](const std::unique_ptr<GameObject>& obj) {
                                        return obj.get() == gameObject;
                                    }),
                                gameObjects.end()
                            );
                            selectedObject = nullptr;
                        }
                        ImGui::EndPopup();
                    }
                }
            }
            else
            {
                ImGui::Text("No scene loaded");
            }

            // Right-click context menu
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Create Empty GameObject")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Create Sprite")) {}
                if (ImGui::MenuItem("Create Camera")) {}
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    void ImGuiEditorSystem::RenderInspector()
    {
        if (ImGui::Begin("Inspector", &showInspector))
        {
            if (selectedObject)
            {
                RenderComponentInspector(selectedObject);
            }
            else
            {
                ImGui::Text("No object selected");
                ImGui::TextWrapped("Select an object from the Scene Hierarchy to inspect its properties.");
            }
        }
        ImGui::End();
    }

    void ImGuiEditorSystem::RenderConsole()
    {
        if (ImGui::Begin("Console", &showConsole))
        {
            ImGui::Text("AronEngine Console");
            ImGui::Separator();

            // Console output area
            if (ImGui::BeginChild("ConsoleOutput", ImVec2(0, -25), true))
            {
                ImGui::Text("[INFO] AronEngine started successfully");
                ImGui::Text("[DEBUG] ImGui Editor System initialized");
                ImGui::Text("[INFO] Scene loaded: Main Scene");
                ImGui::Text("[WARNING] No audio device found");
                ImGui::Text("[ERROR] Failed to load texture: missing_texture.png");
            }
            ImGui::EndChild();

            // Command input
            static char inputBuffer[256] = "";
            ImGui::InputText("##ConsoleInput", inputBuffer, sizeof(inputBuffer));
            ImGui::SameLine();
            if (ImGui::Button("Execute"))
            {
                // Process command
                if (strlen(inputBuffer) > 0)
                {
                    DEBUG_LOG("Console command: " + std::string(inputBuffer));
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
        }
        ImGui::End();
    }

    void ImGuiEditorSystem::RenderAssetBrowser()
    {
        if (ImGui::Begin("Asset Browser", &showAssetBrowser))
        {
            ImGui::Text("Assets");
            ImGui::Separator();

            // Create Assets folder structure
            if (ImGui::TreeNode("Textures"))
            {
                ImGui::Text("(Drag texture files here)");
                ImGui::Text("Supported: .png, .jpg, .bmp");
                ImGui::TreePop();
            }
            
            if (ImGui::TreeNode("Audio"))
            {
                ImGui::Text("(Drag audio files here)");
                ImGui::Text("Supported: .wav, .mp3, .ogg");
                ImGui::TreePop();
            }
            
            if (ImGui::TreeNode("Scripts"))
            {
                ImGui::Text("(C++ scripts)");
                ImGui::TreePop();
            }
            
            if (ImGui::TreeNode("Scenes"))
            {
                ImGui::Selectable("scene.json", false);
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    LoadScene();
                }
                ImGui::TreePop();
            }
            
            if (ImGui::TreeNode("Materials"))
            {
                ImGui::Text("(Create materials here)");
                ImGui::TreePop();
            }

            ImGui::Separator();
            ImGui::Text("Right-click to import assets");
            
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Import Texture..."))
                {
                    // In a real engine, this would open a file dialog
                    DEBUG_LOG("Import texture dialog would open here");
                }
                if (ImGui::MenuItem("Create Material"))
                {
                    DEBUG_LOG("Create material dialog would open here");
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    void ImGuiEditorSystem::RenderSceneView()
    {
        if (ImGui::Begin("Scene View", &showSceneView))
        {
            ImGui::Text("Scene View - 3D/2D Editor");
            ImGui::Separator();

            // Toolbar
            if (ImGui::Button("Move")) {}
            ImGui::SameLine();
            if (ImGui::Button("Rotate")) {}
            ImGui::SameLine();
            if (ImGui::Button("Scale")) {}

            ImGui::Separator();

            // Viewport area
            ImVec2 viewportSize = ImGui::GetContentRegionAvail();
            ImGui::Text("Viewport: %.0fx%.0f", viewportSize.x, viewportSize.y);
            ImGui::Button("Viewport Area", viewportSize);

            // Handle viewport input
            if (ImGui::IsItemHovered())
            {
                // Handle mouse input for scene navigation
            }
        }
        ImGui::End();
    }

    void ImGuiEditorSystem::RenderGameView()
    {
        if (ImGui::Begin("Game View", &showGameView))
        {
            ImGui::Text("Game View - Runtime");
            ImGui::Separator();

            ImVec2 viewportSize = ImGui::GetContentRegionAvail();
            ImGui::Text("Game Viewport: %.0fx%.0f", viewportSize.x, viewportSize.y);
            ImGui::Button("Game Viewport Area", viewportSize);
        }
        ImGui::End();
    }

    void ImGuiEditorSystem::RenderComponentInspector(GameObject* gameObject)
    {
        if (!gameObject) return;

        ImGui::Text("%s%s", Korean::INSPECTOR_GAMEOBJECT, gameObject->GetName().c_str());
        
        // Active checkbox
        bool active = gameObject->IsActive();
        if (ImGui::Checkbox(Korean::INSPECTOR_ACTIVE, &active))
        {
            gameObject->SetActive(active);
        }

        ImGui::Separator();

        // Transform component (always present)
        Transform* transform = gameObject->GetComponent<Transform>();
        if (transform && ImGui::CollapsingHeader(Korean::INSPECTOR_TRANSFORM, ImGuiTreeNodeFlags_DefaultOpen))
        {
            Vector2 pos = transform->GetLocalPosition();
            float position[2] = {pos.x, pos.y};
            if (ImGui::DragFloat2(Korean::INSPECTOR_POSITION, position, 0.1f))
            {
                transform->SetLocalPosition(Vector2(position[0], position[1]));
            }

            float rotation = transform->GetLocalRotation();
            if (ImGui::DragFloat(Korean::INSPECTOR_ROTATION, &rotation, 0.1f))
            {
                transform->SetLocalRotation(rotation);
            }

            Vector2 scale = transform->GetLocalScale();
            float scaleArray[2] = {scale.x, scale.y};
            if (ImGui::DragFloat2(Korean::INSPECTOR_SCALE, scaleArray, 0.01f, 0.01f, 10.0f))
            {
                transform->SetLocalScale(Vector2(scaleArray[0], scaleArray[1]));
            }
        }

        // SpriteRenderer component
        SpriteRenderer* spriteRenderer = gameObject->GetComponent<SpriteRenderer>();
        if (spriteRenderer && ImGui::CollapsingHeader(Korean::INSPECTOR_SPRITE_RENDERER))
        {
            Color color = spriteRenderer->GetColor();
            float colorArray[4] = {color.r, color.g, color.b, color.a};
            if (ImGui::ColorEdit4(Korean::INSPECTOR_COLOR, colorArray))
            {
                spriteRenderer->SetColor(Color(colorArray[0], colorArray[1], colorArray[2], colorArray[3]));
            }
            
            Vector2 size = spriteRenderer->GetSize();
            float sizeArray[2] = {size.x, size.y};
            if (ImGui::DragFloat2(Korean::INSPECTOR_SIZE, sizeArray, 1.0f, 1.0f, 1000.0f))
            {
                spriteRenderer->SetSize(Vector2(sizeArray[0], sizeArray[1]));
            }
            
            // 텍스처 로딩 섹션
            ImGui::Separator();
            
            // 기본 텍스처 선택
            ImGui::Text(Korean::INSPECTOR_DEFAULT_TEXTURES);
            const char* defaultTextures[] = {
                "없음",
                "default_white",
                "default_red", 
                "default_green",
                "default_blue",
                "default_yellow"
            };
            
            static int selectedDefault = 0;
            if (ImGui::Combo("##DefaultTextures", &selectedDefault, defaultTextures, IM_ARRAYSIZE(defaultTextures)))
            {
                if (selectedDefault > 0) {
                    std::string texName = defaultTextures[selectedDefault];
                    auto texture = TextureManager::GetInstance().GetTexture(texName);
                    if (texture) {
                        auto sprite = std::make_shared<Sprite>(texture, texName);
                        spriteRenderer->SetSprite(sprite);
                        DEBUG_LOG("Default texture applied: " + texName);
                    }
                } else {
                    spriteRenderer->SetSprite(nullptr);
                }
            }
            
            ImGui::Text(Korean::INSPECTOR_TEXTURE_PATH);
            
            static char texturePathBuffer[512] = "";
            std::string currentPath = spriteRenderer->GetTexturePath();
            if (!currentPath.empty()) {
                strncpy_s(texturePathBuffer, currentPath.c_str(), sizeof(texturePathBuffer) - 1);
            }
            
            ImGui::InputText("##TexturePath", texturePathBuffer, sizeof(texturePathBuffer));
            
            ImGui::SameLine();
            if (ImGui::Button(Korean::INSPECTOR_LOAD_TEXTURE))
            {
                std::string texturePath(texturePathBuffer);
                if (!texturePath.empty()) {
                    if (spriteRenderer->LoadSpriteFromFile(texturePath)) {
                        DEBUG_LOG("Successfully loaded texture: " + texturePath);
                    } else {
                        DEBUG_LOG("Failed to load texture: " + texturePath);
                    }
                }
            }
            
            if (!currentPath.empty()) {
                ImGui::SameLine();
                if (ImGui::Button(Korean::INSPECTOR_CLEAR_TEXTURE))
                {
                    spriteRenderer->SetSprite(nullptr);
                    texturePathBuffer[0] = '\0';
                }
                
                // 현재 로드된 텍스처 정보 표시
                auto sprite = spriteRenderer->GetSprite();
                if (sprite && sprite->texture) {
                    ImGui::Text("현재 텍스처: %dx%d", 
                        sprite->texture->size.width, 
                        sprite->texture->size.height);
                }
            }
        }
    }

    void ImGuiEditorSystem::SetupImGuiStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();
        
        // 한글 폰트 로드 시도
        io.Fonts->AddFontDefault();
        
        // 시스템 폰트에서 한글 폰트 찾기
        ImFontConfig font_config;
        font_config.OversampleH = 2;
        font_config.OversampleV = 1;
        font_config.PixelSnapH = true;
        
        // Windows 시스템 한글 폰트들 시도
        const char* korean_fonts[] = {
            "C:/Windows/Fonts/malgun.ttf",    // 맑은 고딕
            "C:/Windows/Fonts/gulim.ttc",     // 굴림
            "C:/Windows/Fonts/batang.ttc",    // 바탕
            "C:/Windows/Fonts/NanumGothic.ttf" // 나눔고딕 (있다면)
        };
        
        for (const char* font_path : korean_fonts) {
            FILE* test_file = nullptr;
            fopen_s(&test_file, font_path, "rb");
            if (test_file) {
                fclose(test_file);
                
                // 한글 범위 추가
                static const ImWchar korean_ranges[] = {
                    0x0020, 0x00FF, // Basic Latin + Latin Supplement
                    0xAC00, 0xD7AF, // Hangul Syllables
                    0x3131, 0x3163, // Hangul Compatibility Jamo
                    0x1100, 0x11FF, // Hangul Jamo
                    0,
                };
                
                ImFont* korean_font = io.Fonts->AddFontFromFileTTF(font_path, 16.0f, &font_config, korean_ranges);
                if (korean_font) {
                    DEBUG_LOG("Korean font loaded: " + std::string(font_path));
                    break;
                }
            }
        }
        
        // Dark theme
        ImGui::StyleColorsDark();
        
        // Customize colors
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.0f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);

        // Rounding
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 4.0f;
    }

    void ImGuiEditorSystem::SetupDocking()
    {
        ImGuiIO& io = ImGui::GetIO();
        if (!(io.ConfigFlags & ImGuiConfigFlags_DockingEnable))
            return;

        // Create main viewport dockspace
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        static bool dockspaceOpen = true;
        ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags);
        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        ImGui::End();
    }

    bool ImGuiEditorSystem::ProcessWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (!isEnabled || !imguiContext) return false;

        ImGui::SetCurrentContext(imguiContext);
        
        // Forward declare the ImGui Win32 handler
        extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        return false;
    }

    GameObject* ImGuiEditorSystem::CreateGameObject(const std::string& name)
    {
        auto gameObject = std::make_unique<GameObject>(name);
        
        // Add Transform component (required for all GameObjects)
        gameObject->AddComponent<Transform>();
        
        GameObject* ptr = gameObject.get();
        gameObjects.push_back(std::move(gameObject));
        
        DEBUG_LOG("GameObject created: " + name);
        return ptr;
    }

    GameObject* ImGuiEditorSystem::CreateSpriteObject(const std::string& name)
    {
        GameObject* obj = CreateGameObject(name);
        
        // Add SpriteRenderer component
        obj->AddComponent<SpriteRenderer>();
        
        DEBUG_LOG("Sprite GameObject created: " + name);
        return obj;
    }

    GameObject* ImGuiEditorSystem::CreateSquareObject(const std::string& name)
    {
        GameObject* obj = CreateGameObject(name);
        
        // Add SpriteRenderer for square shape
        auto spriteRenderer = obj->AddComponent<SpriteRenderer>();
        spriteRenderer->SetColor(Color::Blue);
        
        DEBUG_LOG("Square GameObject created: " + name);
        return obj;
    }

    GameObject* ImGuiEditorSystem::CreateCircleObject(const std::string& name)
    {
        GameObject* obj = CreateGameObject(name);
        
        // Add SpriteRenderer for circle shape (we'll render as circle in renderer)
        auto spriteRenderer = obj->AddComponent<SpriteRenderer>();
        spriteRenderer->SetColor(Color::Red);
        
        DEBUG_LOG("Circle GameObject created: " + name);
        return obj;
    }

    void ImGuiEditorSystem::NewScene()
    {
        gameObjects.clear();
        selectedObject = nullptr;
        DEBUG_LOG("New scene created");
    }

    void ImGuiEditorSystem::SaveScene()
    {
        std::ofstream file("scene.json");
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open scene.json for writing");
            return;
        }

        file << "{\n";
        file << "  \"gameObjects\": [\n";

        for (size_t i = 0; i < gameObjects.size(); ++i)
        {
            auto& obj = gameObjects[i];
            file << "    {\n";
            file << "      \"name\": \"" << obj->GetName() << "\",\n";
            file << "      \"active\": " << (obj->IsActive() ? "true" : "false") << ",\n";

            // Transform component
            Transform* transform = obj->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetLocalPosition();
                Vector2 scale = transform->GetLocalScale();
                float rotation = transform->GetLocalRotation();
                
                file << "      \"transform\": {\n";
                file << "        \"position\": [" << pos.x << ", " << pos.y << "],\n";
                file << "        \"rotation\": " << rotation << ",\n";
                file << "        \"scale\": [" << scale.x << ", " << scale.y << "]\n";
                file << "      }";
            }

            // SpriteRenderer component
            SpriteRenderer* spriteRenderer = obj->GetComponent<SpriteRenderer>();
            if (spriteRenderer)
            {
                Color color = spriteRenderer->GetColor();
                Vector2 size = spriteRenderer->GetSize();
                
                file << ",\n      \"spriteRenderer\": {\n";
                file << "        \"color\": [" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << "],\n";
                file << "        \"size\": [" << size.x << ", " << size.y << "]\n";
                file << "      }";
            }

            file << "\n    }";
            if (i < gameObjects.size() - 1) file << ",";
            file << "\n";
        }

        file << "  ]\n";
        file << "}\n";
        file.close();

        DEBUG_LOG("Scene saved to scene.json");
    }

    void ImGuiEditorSystem::LoadScene()
    {
        std::ifstream file("scene.json");
        if (!file.is_open())
        {
            DEBUG_LOG("Scene file not found - creating empty scene");
            return;
        }

        // Clear current scene
        gameObjects.clear();
        selectedObject = nullptr;

        // Simple JSON parsing (in a real engine you'd use a proper JSON library)
        std::string line;
        std::string content;
        while (std::getline(file, line))
        {
            content += line + "\n";
        }
        file.close();

        // This is a very basic JSON parser - in a real engine use nlohmann/json or similar
        size_t pos = 0;
        while ((pos = content.find("\"name\": \"", pos)) != std::string::npos)
        {
            pos += 9; // Skip '"name": "'
            size_t endPos = content.find('"', pos);
            if (endPos == std::string::npos) break;
            
            std::string name = content.substr(pos, endPos - pos);
            
            // Create appropriate GameObject based on name
            GameObject* obj;
            if (name.find("Circle") != std::string::npos)
                obj = CreateCircleObject(name);
            else if (name.find("Square") != std::string::npos)
                obj = CreateSquareObject(name);
            else if (name.find("Sprite") != std::string::npos)
                obj = CreateSpriteObject(name);
            else
                obj = CreateGameObject(name);

            // Parse transform data (simplified parsing)
            size_t transformPos = content.find("\"transform\":", pos);
            if (transformPos != std::string::npos && transformPos < content.find("\"name\":", pos + 1))
            {
                Transform* transform = obj->GetComponent<Transform>();
                if (transform)
                {
                    // Parse position
                    size_t positionPos = content.find("\"position\": [", transformPos);
                    if (positionPos != std::string::npos)
                    {
                        positionPos += 13;
                        size_t commaPos = content.find(',', positionPos);
                        size_t endBracketPos = content.find(']', commaPos);
                        
                        if (commaPos != std::string::npos && endBracketPos != std::string::npos)
                        {
                            float x = std::stof(content.substr(positionPos, commaPos - positionPos));
                            float y = std::stof(content.substr(commaPos + 2, endBracketPos - commaPos - 2));
                            transform->SetLocalPosition(Vector2(x, y));
                        }
                    }
                }
            }

            pos = endPos;
        }

        DEBUG_LOG("Loaded " + std::to_string(gameObjects.size()) + " objects from scene.json");
    }
}