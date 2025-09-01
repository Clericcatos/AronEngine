#pragma once
#include "Core/Singleton.h"
#include "Utils/Vector2.h"
#include "Utils/Vector3.h"
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
        Vector2 consolePos, consoleSize;
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
        bool showWindowMenu;
        bool showHelpMenu;
        
        // 패널 가시성 상태
        bool showHierarchyPanel;
        bool showScenePanel;
        bool showInspectorPanel;
        bool showProjectPanel;
        bool showConsolePanel;
        
        // 텍스트 편집
        bool isEditingText;
        std::string editingField;
        std::string tempEditValue;
        
        // 클릭 처리 상태
        bool lastClickProcessed;
        Vector2 lastClickPos;

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
        void DrawWindowMenu(Renderer* renderer);
        void DrawHelpMenu(Renderer* renderer);
        void DrawConsole(Renderer* renderer);
        
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
        
        // Menu Click Handlers
        bool HandleFileMenuClick(Vector2 mousePos);
        bool HandleEditMenuClick(Vector2 mousePos);
        bool HandleGameObjectMenuClick(Vector2 mousePos);
        bool HandleComponentMenuClick(Vector2 mousePos);
        bool HandleWindowMenuClick(Vector2 mousePos);
        bool HandleHelpMenuClick(Vector2 mousePos);
        
        // File Menu Actions
        void CreateNewScene();
        void OpenSceneDialog();
        void SaveCurrentScene();
        void SaveAsDialog();
        void ShowBuildSettings();
        void BuildAndRun();
        void ExitApplication();
        
        // Edit Menu Actions
        void UndoAction();
        void RedoAction();
        void CutSelected();
        void CopySelected();
        void PasteObject();
        void DuplicateSelected();
        
        // GameObject Menu Actions
        void CreateChildObject();
        void CreateCameraObject(Vector2 position);
        void CreateAudioSourceObject(Vector2 position);
        
        // Component Menu Actions
        void AddSpriteRenderer();
        void AddAudioSource();
        void AddRigidbody();
        void AddCollider();
        void AddScript();
        
        // Scene Management
        void SaveSceneToFile(const std::string& fileName);
        void LoadSceneFromFile(const std::string& fileName);
        
        // Window Menu Actions
        void ToggleHierarchyPanel();
        void ToggleScenePanel();
        void ToggleInspectorPanel();
        void ToggleProjectPanel();
        void ToggleConsolePanel();
        
        // Help Menu Actions
        void ShowAboutDialog();
        void ShowDocumentation();
        
        bool IsPointInRect(Vector2 point, Vector2 pos, Vector2 size);
    };
}