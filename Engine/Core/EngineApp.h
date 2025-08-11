#pragma once
#include "framework.h"
#include "Singleton.h"

namespace AronEngine
{
    class Time;
    class Input;
    class Renderer;
    class SceneManager;
    class ResourceManager;
    class RenderSystem;
    class TransformSystem;
    class ScriptSystem;
    class EditorSystem;

    class EngineApp : public Singleton<EngineApp>
    {
        friend class Singleton<EngineApp>;

    protected:
        HINSTANCE hInstance;
        HWND hWnd;
        int width;
        int height;
        std::wstring windowTitle;
        std::wstring windowClassName;
        bool isRunning;
        bool isEditorMode;

        std::unique_ptr<Time> time;
        std::unique_ptr<Input> input;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<SceneManager> sceneManager;
        std::unique_ptr<ResourceManager> resourceManager;
        std::unique_ptr<RenderSystem> renderSystem;
        std::unique_ptr<TransformSystem> transformSystem;
        std::unique_ptr<ScriptSystem> scriptSystem;
        std::unique_ptr<EditorSystem> editorSystem;

    protected:
        EngineApp();
        virtual ~EngineApp();

    public:
        bool Initialize(HINSTANCE hInstance, int width, int height, const std::wstring& title, bool editorMode = true);
        void Run();
        void Shutdown();

        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnShutdown() {}

        HWND GetWindowHandle() const { return hWnd; }
        int GetWidth() const { return width; }
        int GetHeight() const { return height; }
        bool IsEditorMode() const { return isEditorMode; }
        void Quit() { isRunning = false; }

        Time* GetTime() const { return time.get(); }
        Input* GetInput() const { return input.get(); }
        Renderer* GetRenderer() const { return renderer.get(); }
        SceneManager* GetSceneManager() const { return sceneManager.get(); }
        ResourceManager* GetResourceManager() const { return resourceManager.get(); }
        EditorSystem* GetEditorSystem() const { return editorSystem.get(); }

    private:
        bool CreateAppWindow();
        void Update(float deltaTime);
        void Render();
        void ProcessMessages();

        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    };
}