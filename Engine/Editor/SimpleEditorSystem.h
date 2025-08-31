#pragma once
#include "Core/Singleton.h"
#include "Utils/Vector2.h"
#include "Utils/Color.h"
#include <windows.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace AronEngine
{
    class GameObject;
    class Renderer;

    struct EditorButton
    {
        Vector2 position;
        Vector2 size;
        std::wstring text;
        bool isPressed;
        std::function<void()> onClick;
        
        EditorButton(Vector2 pos, Vector2 sz, std::wstring txt)
            : position(pos), size(sz), text(txt), isPressed(false) {}
    };

    class SimpleEditorSystem : public Singleton<SimpleEditorSystem>
    {
        friend class Singleton<SimpleEditorSystem>;

    private:
        bool isEnabled;
        bool showHierarchy;
        bool showInspector;
        
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        GameObject* selectedObject;
        
        std::vector<EditorButton> buttons;
        Vector2 mousePos;
        bool mousePressed;
        bool rightClickPressed;
        bool showCreateMenu;
        Vector2 createMenuPos;

    protected:
        SimpleEditorSystem();
        virtual ~SimpleEditorSystem();

    public:
        bool Initialize();
        void Shutdown();
        
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        void SetEnabled(bool enabled) { isEnabled = enabled; }
        bool IsEnabled() const { return isEnabled; }
        
        const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return gameObjects; }
        GameObject* GetSelectedObject() const { return selectedObject; }
        
        void HandleMouseInput(Vector2 mousePosition, bool leftPressed, bool rightPressed);
        
    private:
        void RenderHierarchy(Renderer* renderer);
        void RenderInspector(Renderer* renderer);
        void RenderToolbar(Renderer* renderer);
        void RenderButtons(Renderer* renderer);
        
        GameObject* CreateGameObject(const std::string& name);
        GameObject* CreateSpriteObject(const std::string& name);
        GameObject* CreateCubeObject(const std::string& name);
        GameObject* CreateEmptyGameObject(const std::string& name);
        void DeleteSelectedObject();
        
        bool IsPointInRect(Vector2 point, Vector2 rectPos, Vector2 rectSize);
        void SetupButtons();
    };
}