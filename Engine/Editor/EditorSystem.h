#pragma once
#include "../Core/framework.h"
#include "../Utils/Vector2.h"
#include <memory>

struct ImGuiContext;

namespace AronEngine
{
    class Renderer;
    class GameObject;
    class Scene;

    class EditorSystem
    {
    private:
        bool isEnabled;
        bool showDemoWindow;
        bool showSceneHierarchy;
        bool showInspector;
        bool showConsole;
        bool showSceneView;
        bool showGameView;
        bool showAssetBrowser;
        
        HWND hWnd;
        Renderer* renderer;
        
        GameObject* selectedObject;
        Scene* currentScene;
        
        Vector2 sceneViewSize;
        Vector2 gameViewSize;
        
        bool isDragging;
        Vector2 dragStartPos;
        Vector2 dragCurrentPos;

    public:
        EditorSystem();
        ~EditorSystem();

        bool Initialize(HWND hWnd, Renderer* renderer);
        void Shutdown();
        
        void Update(float deltaTime);
        void Render();

        bool ProcessWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        
        void SetCurrentScene(Scene* scene) { currentScene = scene; }
        GameObject* GetSelectedObject() const { return selectedObject; }
        void SetSelectedObject(GameObject* obj) { selectedObject = obj; }

    private:
        void DrawEditorInterface();
        void DrawMenuBar(int windowWidth);
        void DrawSceneHierarchy(int windowWidth, int windowHeight);
        void DrawInspector(int windowWidth, int windowHeight);
        void DrawConsole(int windowWidth, int windowHeight);
        void DrawGameView(int windowWidth, int windowHeight);
        void DrawSceneView(int windowWidth, int windowHeight);
        
        void RenderMenuBar();
        void RenderSceneHierarchy();
        void RenderInspector();
        void RenderConsole();
        void RenderGameView();
        void RenderSceneView();
        void RenderAssetBrowser();
        
        void RenderGameObjectNode(GameObject* gameObject);
        void HandleSceneViewInput();
        void DrawGrid();
        void DrawGizmos();
        
        GameObject* CreatePrimitiveObject(const std::string& name, const std::string& type);
        void DeleteSelectedObject();
    };
}