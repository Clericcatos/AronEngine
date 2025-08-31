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
        std::string type;
        std::shared_ptr<Texture> texture;
        std::shared_ptr<AudioClip> audio;
    };

    class LightUnityEditor : public Singleton<LightUnityEditor>
    {
        friend class Singleton<LightUnityEditor>;

    private:
        bool isEnabled;
        std::vector<std::unique_ptr<GameObject>> sceneObjects;
        GameObject* selectedObject;
        
        int screenWidth;
        int screenHeight;
        
        Vector2 hierarchyPos, hierarchySize;
        Vector2 scenePos, sceneSize;
        Vector2 inspectorPos, inspectorSize;
        Vector2 projectPos, projectSize;
        Vector2 toolbarPos, toolbarSize;
        
        std::vector<AssetItem> assets;
        std::string projectPath;
        
        bool showCreateMenu;
        Vector2 createMenuPos;
        bool isDragging;
        GameObject* draggedObject;
        Vector2 dragOffset;
        
        // 에디터 상태
        int selectedIndex;
        bool isPlayMode;
        int selectedTool;
        std::string searchQuery;
        
        // 메뉴 상태
        bool showFileMenu;
        bool showEditMenu;
        bool showGameObjectMenu;
        bool showComponentMenu;
        
        // 텍스트 편집
        bool isEditingText;
        std::string editingField;
        std::string tempEditValue;

    protected:
        LightUnityEditor();
        virtual ~LightUnityEditor();

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
        void HandleChar(wchar_t ch);
        
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
        
        void StartTextEdit(const std::string& field, const std::string& currentValue);
        void EndTextEdit(bool apply);
        
        void SaveScene(const std::string& fileName);
        void LoadScene(const std::string& fileName);
        
        bool IsPointInRect(Vector2 point, Vector2 pos, Vector2 size);
    };
}