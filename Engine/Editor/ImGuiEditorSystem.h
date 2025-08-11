#pragma once
#include "Core/Singleton.h"
#include <windows.h>
#include <d3d11.h>
#include <memory>
#include <vector>

// Forward declarations
struct ImGuiContext;
struct ImGuiIO;

namespace AronEngine
{
    class Scene;
    class GameObject;
    class Renderer;

    class ImGuiEditorSystem : public Singleton<ImGuiEditorSystem>
    {
        friend class Singleton<ImGuiEditorSystem>;

    private:
        HWND hWnd;
        Renderer* renderer;
        bool isEnabled;
        bool showDemoWindow;
        bool showSceneHierarchy;
        bool showInspector;
        bool showConsole;
        bool showAssetBrowser;
        bool showGameView;
        bool showSceneView;

        // ImGui context
        ImGuiContext* imguiContext;
        
        // D3D11 resources for ImGui
        ID3D11Device* d3d11Device;
        ID3D11DeviceContext* d3d11Context;
        
        // Editor state
        GameObject* selectedObject;
        Scene* currentScene;
        std::vector<std::unique_ptr<GameObject>> gameObjects;

        // Layout
        bool isDockingEnabled;
        bool isViewportsEnabled;

    protected:
        ImGuiEditorSystem();
        virtual ~ImGuiEditorSystem();

    public:
        bool Initialize(HWND hWnd, Renderer* renderer);
        void Shutdown();

        void BeginFrame();
        void EndFrame();
        
        void Update(float deltaTime);
        void Render();

        // Window management
        void SetEnabled(bool enabled) { isEnabled = enabled; }
        bool IsEnabled() const { return isEnabled; }

        void ShowDemoWindow(bool show) { showDemoWindow = show; }
        void ShowSceneHierarchy(bool show) { showSceneHierarchy = show; }
        void ShowInspector(bool show) { showInspector = show; }
        void ShowConsole(bool show) { showConsole = show; }
        void ShowAssetBrowser(bool show) { showAssetBrowser = show; }

        // Scene management
        void SetCurrentScene(Scene* scene) { currentScene = scene; }
        Scene* GetCurrentScene() const { return currentScene; }

        void SetSelectedObject(GameObject* obj) { selectedObject = obj; }
        GameObject* GetSelectedObject() const { return selectedObject; }
        
        const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return gameObjects; }

        // Window message processing
        bool ProcessWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        void RenderMainMenuBar();
        void RenderSceneHierarchy();
        void RenderInspector();
        void RenderConsole();
        void RenderAssetBrowser();
        void RenderSceneView();
        void RenderGameView();

        void RenderGameObjectNode(GameObject* gameObject);
        void RenderComponentInspector(GameObject* gameObject);

        void SetupImGuiStyle();
        void SetupDocking();
        
        // GameObject creation methods
        GameObject* CreateGameObject(const std::string& name);
        GameObject* CreateSpriteObject(const std::string& name);
        GameObject* CreateSquareObject(const std::string& name);
        GameObject* CreateCircleObject(const std::string& name);
        
        // Scene management methods
        void NewScene();
        void SaveScene();
        void LoadScene();
    };
}