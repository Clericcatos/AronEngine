#pragma once
#include "Core/Singleton.h"
#include "Utils/Vector2.h"
#include "Utils/Color.h"
#include <windows.h>
#include <memory>
#include <vector>
#include <string>

namespace AronEngine
{
    class GameObject;
    class Renderer;

    class UnityStyleEditor : public Singleton<UnityStyleEditor>
    {
        friend class Singleton<UnityStyleEditor>;

    private:
        bool isEnabled;
        std::vector<std::unique_ptr<GameObject>> sceneObjects;
        GameObject* selectedObject;
        
        // UI state
        bool showCreateDropdown;
        Vector2 createDropdownPos;

    protected:
        UnityStyleEditor();
        virtual ~UnityStyleEditor();

    public:
        bool Initialize();
        void Shutdown();
        
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        void SetEnabled(bool enabled) { isEnabled = enabled; }
        bool IsEnabled() const { return isEnabled; }
        
        const std::vector<std::unique_ptr<GameObject>>& GetSceneObjects() const { return sceneObjects; }
        GameObject* GetSelectedObject() const { return selectedObject; }
        
        void HandleInput(Vector2 mousePos, bool leftClick, bool rightClick);
        
    private:
        void DrawHierarchy(Renderer* renderer);
        void DrawInspector(Renderer* renderer);  
        void DrawToolbar(Renderer* renderer);
        void DrawSceneView(Renderer* renderer);
        void DrawCreateDropdown(Renderer* renderer);
        
        GameObject* CreateEmpty(const std::string& name, Vector2 position);
        GameObject* CreateSprite(const std::string& name, Vector2 position);
        GameObject* CreateCube(const std::string& name, Vector2 position);
        
        void SelectObject(GameObject* obj);
        void DeleteSelected();
        
        bool IsPointInRect(Vector2 point, Vector2 pos, Vector2 size);
    };
}