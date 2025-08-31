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
    struct Texture;
    class AudioClip;

    struct AssetItem
    {
        std::string name;
        std::string path;
        std::string type; // "texture", "audio", "script"
        std::shared_ptr<Texture> texture;
        std::shared_ptr<AudioClip> audio;
    };

    class RealUnityEditor : public Singleton<RealUnityEditor>
    {
        friend class Singleton<RealUnityEditor>;

    private:
        bool isEnabled;
        std::vector<std::unique_ptr<GameObject>> sceneObjects;
        GameObject* selectedObject;
        
        // Screen layout (real Unity proportions)
        int screenWidth;
        int screenHeight;
        
        // Panel dimensions (Unity-accurate)
        Vector2 hierarchyPos, hierarchySize;
        Vector2 scenePos, sceneSize;
        Vector2 inspectorPos, inspectorSize;
        Vector2 projectPos, projectSize;
        Vector2 toolbarPos, toolbarSize;
        
        // Asset management
        std::vector<AssetItem> assets;
        std::string projectPath;
        
        // UI state
        bool showCreateMenu;
        Vector2 createMenuPos;
        bool isDragging;
        GameObject* draggedObject;
        Vector2 dragOffset;
        
        // Editor state
        int selectedIndex;
        bool isPlayMode;
        int selectedTool; // 0=Hand, 1=Move, 2=Rotate, 3=Scale, 4=Rect
        std::string searchQuery;
        
        // Menu state
        bool showFileMenu;
        bool showEditMenu;
        bool showGameObjectMenu;
        bool showComponentMenu;

    protected:
        RealUnityEditor();
        virtual ~RealUnityEditor();

    public:
        bool Initialize(int width, int height);
        void Shutdown();
        
        void Update(float deltaTime);
        void Render(Renderer* renderer);
        
        void SetEnabled(bool enabled) { isEnabled = enabled; }
        bool IsEnabled() const { return isEnabled; }
        
        const std::vector<std::unique_ptr<GameObject>>& GetSceneObjects() const { return sceneObjects; }
        GameObject* GetSelectedObject() const { return selectedObject; }
        
        void HandleInput(Vector2 mousePos, bool leftClick, bool rightClick);
        void HandleDragDrop(const std::wstring& filePath);
        void HandleKeyInput(int keyCode, bool shift, bool ctrl);
        
        // File operations
        void LoadImageFile(const std::string& filePath);
        void LoadAudioFile(const std::string& filePath);
        void RefreshAssets();
        
    private:
        void CalculateLayout();
        void DrawToolbar(Renderer* renderer);
        void DrawHierarchy(Renderer* renderer);
        void DrawSceneView(Renderer* renderer);
        void DrawInspector(Renderer* renderer);
        void DrawProject(Renderer* renderer);
        void DrawCreateMenu(Renderer* renderer);
        void DrawFileMenu(Renderer* renderer);
        void DrawEditMenu(Renderer* renderer);
        void DrawGameObjectMenu(Renderer* renderer);
        void DrawComponentMenu(Renderer* renderer);
        
        GameObject* CreateEmptyGameObject(const std::string& name, Vector2 position);
        GameObject* CreateSpriteGameObject(const std::string& name, Vector2 position);
        
        void SelectObject(GameObject* obj);
        void DeleteSelected();
        void StartDrag(GameObject* obj, Vector2 mousePos);
        void UpdateDrag(Vector2 mousePos);
        void EndDrag();
        
        bool IsPointInRect(Vector2 point, Vector2 pos, Vector2 size);
    };
}