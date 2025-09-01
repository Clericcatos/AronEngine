#include "LightUnityEditor.h"
#include "KoreanUI.h"
#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Core/TextureManager.h"
#include "Components/Transform.h"
#include "Components/SpriteRenderer.h"
#include "Components/AudioSource.h"
#include "Components/Camera3D.h"
#include "Components/MeshRenderer.h"
#include "Resources/AudioClip.h"
#include "Resources/Texture2D.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"
#include "Core/GameEngine.h"
#include "Utils/Color.h"
#include <shellapi.h>
#include <commdlg.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cwchar>

namespace AronEngine
{
    LightUnityEditor::LightUnityEditor()
        : isEnabled(false)
        , selectedObject(nullptr)
        , screenWidth(1024)
        , screenHeight(768)
        , showCreateMenu(false)
        , createMenuPos(0, 0)
        , isDragging(false)
        , draggedObject(nullptr)
        , dragOffset(0, 0)
        , selectedIndex(-1)
        , isPlayMode(false)
        , selectedTool(0)
        , showFileMenu(false)
        , showEditMenu(false)
        , showGameObjectMenu(false)
        , showComponentMenu(false)
        , showWindowMenu(false)
        , showHelpMenu(false)
        , showHierarchyPanel(true)
        , showScenePanel(true)
        , showInspectorPanel(true)
        , showProjectPanel(true)
        , showConsolePanel(false)
        , isEditingText(false)
        , editingField("")
        , tempEditValue("")
        , lastClickProcessed(false)
        , lastClickPos(0, 0)
        , projectPath("Assets")
    {
    }

    LightUnityEditor::~LightUnityEditor()
    {
    }

    bool LightUnityEditor::Initialize(int width, int height)
    {
        screenWidth = width;
        screenHeight = height;
        
        CalculateLayout();
        RefreshAssets();
        
        return true;
    }

    void LightUnityEditor::Shutdown()
    {
        sceneObjects.clear();
        selectedObject = nullptr;
        assets.clear();
    }

    void LightUnityEditor::Update(float deltaTime)
    {
        // Update logic for editor
    }

    void LightUnityEditor::CalculateLayout()
    {
        const float menuHeight = 30.0f;
        const float hierarchyWidth = showHierarchyPanel ? 250.0f : 0.0f;
        const float inspectorWidth = showInspectorPanel ? 300.0f : 0.0f;
        const float projectHeight = showProjectPanel ? 200.0f : 0.0f;
        const float consoleHeight = showConsolePanel ? 150.0f : 0.0f;
        
        toolbarPos = Vector2(0, 0);
        toolbarSize = Vector2((float)screenWidth, menuHeight);
        
        float currentY = menuHeight;
        float availableHeight = (float)screenHeight - menuHeight - projectHeight - consoleHeight;
        
        if (showHierarchyPanel)
        {
            hierarchyPos = Vector2(0, currentY);
            hierarchySize = Vector2(hierarchyWidth, availableHeight);
        }
        
        if (showScenePanel)
        {
            scenePos = Vector2(hierarchyWidth, currentY);
            sceneSize = Vector2((float)screenWidth - hierarchyWidth - inspectorWidth, availableHeight);
        }
        
        if (showInspectorPanel)
        {
            inspectorPos = Vector2((float)screenWidth - inspectorWidth, currentY);
            inspectorSize = Vector2(inspectorWidth, availableHeight);
        }
        
        if (showProjectPanel)
        {
            projectPos = Vector2(0, (float)screenHeight - projectHeight - consoleHeight);
            projectSize = Vector2((float)screenWidth, projectHeight);
        }
        
        if (showConsolePanel)
        {
            consolePos = Vector2(0, (float)screenHeight - consoleHeight);
            consoleSize = Vector2((float)screenWidth, consoleHeight);
        }
    }

    void LightUnityEditor::Render(Renderer* renderer)
    {
        if (!isEnabled) return;
        
        DrawToolbar(renderer);
        DrawHierarchy(renderer);
        DrawSceneView(renderer);
        DrawInspector(renderer);
        DrawProject(renderer);
        DrawConsole(renderer);
        
        if (showCreateMenu)
            DrawCreateMenu(renderer);
            
        if (showFileMenu)
            DrawFileMenu(renderer);
            
        if (showEditMenu)
            DrawEditMenu(renderer);
            
        if (showGameObjectMenu)
            DrawGameObjectMenu(renderer);
            
        if (showComponentMenu)
            DrawComponentMenu(renderer);
            
        if (showWindowMenu)
            DrawWindowMenu(renderer);
            
        if (showHelpMenu)
            DrawHelpMenu(renderer);
    }

    void LightUnityEditor::DrawToolbar(Renderer* renderer)
    {
        renderer->FillRectangle(toolbarPos, toolbarSize, Color(0.95f, 0.95f, 0.95f));
        renderer->DrawLine(Vector2(0, toolbarSize.y), Vector2((float)screenWidth, toolbarSize.y), Color(0.8f, 0.8f, 0.8f), 1.0f);
        
        Vector2 menuPos(8, 8);
        const wchar_t* menus[] = {UI::MENU_FILE, UI::MENU_EDIT, UI::MENU_ASSETS, UI::MENU_GAMEOBJECT, UI::MENU_COMPONENT, UI::MENU_WINDOW, UI::MENU_HELP};
        
        for (int i = 0; i < 7; i++)
        {
            renderer->DrawText(menus[i], menuPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 10.0f);
            menuPos.x += 80;
        }
        
        Vector2 btnPos = toolbarPos + Vector2(600, 4);
        Vector2 btnSize(28, 22);
        
        const wchar_t* tools[] = {L"Q", L"W", L"E", L"R", L"T"};
        
        for (int i = 0; i < 5; i++)
        {
            bool isSelected = (i == selectedTool);
            Color btnColor = isSelected ? Color(0.3f, 0.5f, 0.9f) : Color(0.88f, 0.88f, 0.88f);
            
            renderer->FillRectangle(btnPos, btnSize, btnColor);
            renderer->DrawRectangle(btnPos, btnSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
            Color textColor = isSelected ? Color::White : Color(0.1f, 0.1f, 0.1f);
            renderer->DrawText(tools[i], btnPos + Vector2(10, 4), textColor, L"Arial", 10.0f);
            
            btnPos.x += 30;
        }
        
        Vector2 playBtnPos((float)screenWidth / 2 - 45, 4);
        Vector2 playBtnSize(90, 22);
        Color playBtnColor = isPlayMode ? Color(0.2f, 0.8f, 0.2f) : Color(0.88f, 0.88f, 0.88f);
        
        renderer->FillRectangle(playBtnPos, playBtnSize, playBtnColor);
        renderer->DrawRectangle(playBtnPos, playBtnSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        const wchar_t* playText = isPlayMode ? L"||" : L">";
        Color playTextColor = isPlayMode ? Color::White : Color(0.1f, 0.1f, 0.1f);
        Vector2 playCenter = playBtnPos + playBtnSize / 2;
        renderer->DrawText(playText, playCenter - Vector2(8, 8), playTextColor, L"Arial", 12.0f);
    }

    void LightUnityEditor::DrawHierarchy(Renderer* renderer)
    {
        if (!showHierarchyPanel) return;
        
        renderer->FillRectangle(hierarchyPos, hierarchySize, Color(0.9f, 0.9f, 0.9f));
        renderer->DrawText(UI::PANEL_HIERARCHY, hierarchyPos + Vector2(8, 4), Color(0.1f, 0.1f, 0.1f), L"Arial", 10.0f);
        
        Vector2 searchPos = hierarchyPos + Vector2(8, 25);
        Vector2 searchSize(hierarchySize.x - 16, 20);
        
        renderer->FillRectangle(searchPos, searchSize, Color(0.95f, 0.95f, 0.95f));
        renderer->DrawRectangle(searchPos, searchSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        renderer->DrawText(UI::UI_SEARCH, searchPos + Vector2(4, 4), Color(0.4f, 0.4f, 0.4f), L"Arial", 9.0f);
        
        Vector2 objListPos = hierarchyPos + Vector2(8, 50);
        Vector2 objListSize(hierarchySize.x - 16, hierarchySize.y - 55);
        
        renderer->FillRectangle(objListPos, objListSize, Color(0.93f, 0.93f, 0.93f));
        renderer->DrawRectangle(objListPos, objListSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        if (sceneObjects.empty())
        {
            Vector2 centerPos = objListPos + objListSize / 2;
            renderer->DrawText(UI::UI_NO_OBJECTS, centerPos - Vector2(80, 0), Color(0.4f, 0.4f, 0.4f), L"Arial", 9.0f);
            renderer->DrawText(UI::UI_RIGHT_CLICK_CREATE, centerPos - Vector2(80, -20), Color(0.4f, 0.4f, 0.4f), L"Arial", 8.0f);
        }
        else
        {
            Vector2 itemPos = objListPos + Vector2(4, 4);
            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                GameObject* obj = sceneObjects[i].get();
                bool isSelected = (obj == selectedObject);
                
                if (isSelected)
                {
                    renderer->FillRectangle(itemPos, Vector2(objListSize.x - 8, 18), Color(0.3f, 0.5f, 0.9f));
                }
                
                Color textColor = isSelected ? Color::White : Color(0.1f, 0.1f, 0.1f);
                std::wstring objName = std::wstring(obj->GetName().begin(), obj->GetName().end());
                renderer->DrawText(objName.c_str(), itemPos + Vector2(4, 2), textColor, L"Arial", 9.0f);
                
                itemPos.y += 20;
            }
        }
    }

    void LightUnityEditor::DrawSceneView(Renderer* renderer)
    {
        if (!showScenePanel) return;
        
        renderer->FillRectangle(scenePos, sceneSize, Color(0.7f, 0.7f, 0.7f));
        
        Vector2 headerPos = scenePos;
        Vector2 headerSize(sceneSize.x, 24);
        renderer->FillRectangle(headerPos, headerSize, Color(0.88f, 0.88f, 0.88f));
        renderer->DrawText(UI::PANEL_SCENE, headerPos + Vector2(8, 4), Color(0.1f, 0.1f, 0.1f), L"Arial", 10.0f);
        
        Vector2 tabPos = headerPos + Vector2(60, 0);
        Vector2 tabSize(80, 24);
        renderer->FillRectangle(tabPos, tabSize, Color(0.95f, 0.95f, 0.95f));
        renderer->DrawText(L"New Scene", tabPos + Vector2(8, 4), Color(0.1f, 0.1f, 0.1f), L"Arial", 9.0f);
        
        Vector2 gridPos = scenePos + Vector2(0, 24);
        Vector2 gridSize = sceneSize - Vector2(0, 24);
        
        renderer->FillRectangle(gridPos, gridSize, Color(0.4f, 0.4f, 0.4f));
        
        const float gridSpacing = 50.0f;
        Color gridColor(0.5f, 0.5f, 0.5f);
        
        for (float x = gridPos.x; x < gridPos.x + gridSize.x; x += gridSpacing)
        {
            renderer->DrawLine(Vector2(x, gridPos.y), Vector2(x, gridPos.y + gridSize.y), gridColor, 1.0f);
        }
        
        for (float y = gridPos.y; y < gridPos.y + gridSize.y; y += gridSpacing)
        {
            renderer->DrawLine(Vector2(gridPos.x, y), Vector2(gridPos.x + gridSize.x, y), gridColor, 1.0f);
        }
        
        // Render scene objects in the scene view
        for (auto& obj : sceneObjects)
        {
            auto transform = obj->GetComponent<Transform>();
            if (transform)
            {
                Vector2 objPos = transform->GetLocalPosition();
                Vector2 screenPos = gridPos + objPos; // Convert to screen coordinates
                
                // Draw object representation
                bool isSelected = (obj.get() == selectedObject);
                Color objColor = isSelected ? Color(1.0f, 0.5f, 0.0f) : Color(0.8f, 0.8f, 0.8f);
                
                renderer->FillRectangle(screenPos, Vector2(32, 32), objColor);
                renderer->DrawRectangle(screenPos, Vector2(32, 32), Color(0.2f, 0.2f, 0.2f), 2.0f);
                
                // Draw object name
                std::wstring objName = std::wstring(obj->GetName().begin(), obj->GetName().end());
                if (objName.length() > 8)
                    objName = objName.substr(0, 6) + L"..";
                    
                renderer->DrawText(objName.c_str(), screenPos + Vector2(2, 34), Color(0.9f, 0.9f, 0.9f), L"Arial", 8.0f);
            }
        }
    }

    void LightUnityEditor::DrawInspector(Renderer* renderer)
    {
        if (!showInspectorPanel) return;
        
        renderer->FillRectangle(inspectorPos, inspectorSize, Color(0.9f, 0.9f, 0.9f));
        renderer->DrawText(UI::PANEL_INSPECTOR, inspectorPos + Vector2(8, 4), Color(0.1f, 0.1f, 0.1f), L"Arial", 10.0f);
        
        if (!selectedObject)
        {
            Vector2 contentPos = inspectorPos + Vector2(8, 30);
            renderer->DrawText(UI::UI_NOTHING_SELECTED, contentPos, Color(0.4f, 0.4f, 0.4f), L"Arial", 10.0f);
        }
        else
        {
            Vector2 contentPos = inspectorPos + Vector2(8, 30);
            
            std::wstring objName = std::wstring(selectedObject->GetName().begin(), selectedObject->GetName().end());
            renderer->DrawText(objName.c_str(), contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 12.0f);
            
            contentPos.y += 30;
            
            auto transform = selectedObject->GetComponent<Transform>();
            if (transform)
            {
                renderer->DrawText(UI::INSPECTOR_TRANSFORM, contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 11.0f);
                contentPos.y += 25;
                
                Vector2 pos = transform->GetLocalPosition();
                
                renderer->DrawText(UI::INSPECTOR_POSITION, contentPos, Color(0.2f, 0.2f, 0.2f), L"Arial", 9.0f);
                contentPos.y += 18;
                
                std::wstring posXStr = L"X: " + std::to_wstring(pos.x);
                std::wstring posYStr = L"Y: " + std::to_wstring(pos.y);
                renderer->DrawText(posXStr.c_str(), contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 9.0f);
                contentPos.y += 15;
                renderer->DrawText(posYStr.c_str(), contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 9.0f);
                contentPos.y += 25;
            }
            
            auto spriteRenderer = selectedObject->GetComponent<SpriteRenderer>();
            if (spriteRenderer)
            {
                renderer->DrawText(UI::INSPECTOR_SPRITE_RENDERER, contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 11.0f);
                contentPos.y += 25;
                
                Color spriteColor = spriteRenderer->GetColor();
                std::wstring colorStr = L"Color: R:" + std::to_wstring(spriteColor.r) + L" G:" + std::to_wstring(spriteColor.g) + L" B:" + std::to_wstring(spriteColor.b);
                renderer->DrawText(colorStr.c_str(), contentPos, Color(0.2f, 0.2f, 0.2f), L"Arial", 9.0f);
                contentPos.y += 20;
            }
            
            auto camera3D = selectedObject->GetComponent<Camera3D>();
            if (camera3D)
            {
                renderer->DrawText(L"Camera3D Component", contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 11.0f);
                contentPos.y += 25;
                
                DirectX::XMFLOAT3 pos = camera3D->GetPosition();
                std::wstring posStr = L"Position: " + std::to_wstring(pos.x) + L", " + std::to_wstring(pos.y) + L", " + std::to_wstring(pos.z);
                renderer->DrawText(posStr.c_str(), contentPos, Color(0.2f, 0.2f, 0.2f), L"Arial", 9.0f);
                contentPos.y += 20;
                
                std::wstring fovStr = L"FOV: " + std::to_wstring(camera3D->GetFOV());
                renderer->DrawText(fovStr.c_str(), contentPos, Color(0.2f, 0.2f, 0.2f), L"Arial", 9.0f);
                contentPos.y += 20;
            }
            
            auto meshRenderer = selectedObject->GetComponent<MeshRenderer>();
            if (meshRenderer)
            {
                renderer->DrawText(L"MeshRenderer Component", contentPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 11.0f);
                contentPos.y += 25;
                
                bool hasMesh = meshRenderer->GetMesh() != nullptr;
                std::wstring meshStr = L"Mesh: " + std::wstring(hasMesh ? L"Loaded" : L"None");
                renderer->DrawText(meshStr.c_str(), contentPos, Color(0.2f, 0.2f, 0.2f), L"Arial", 9.0f);
                contentPos.y += 20;
            }
            
            contentPos.y += 10;
            renderer->DrawText(UI::INSPECTOR_ADD_COMPONENT, contentPos, Color(0.3f, 0.3f, 0.8f), L"Arial", 10.0f);
        }
    }

    void LightUnityEditor::DrawProject(Renderer* renderer)
    {
        if (!showProjectPanel) return;
        
        renderer->FillRectangle(projectPos, projectSize, Color(0.88f, 0.88f, 0.88f));
        renderer->DrawLine(Vector2(0, projectPos.y), Vector2((float)screenWidth, projectPos.y), Color(0.8f, 0.8f, 0.8f), 1.0f);
        
        Vector2 headerSize(projectSize.x, 20.0f);
        renderer->FillRectangle(projectPos, headerSize, Color(0.85f, 0.85f, 0.85f));
        renderer->DrawText(UI::PANEL_PROJECT, projectPos + Vector2(8, 4), Color(0.1f, 0.1f, 0.1f), L"Arial", 10.0f);
        
        Vector2 pathBarPos = projectPos + Vector2(0, 20);
        Vector2 pathBarSize(projectSize.x, 22.0f);
        renderer->FillRectangle(pathBarPos, pathBarSize, Color(0.93f, 0.93f, 0.93f));
        renderer->DrawText(L"Assets", pathBarPos + Vector2(8, 4), Color(0.2f, 0.2f, 0.2f), L"Arial", 9.0f);
        renderer->DrawLine(Vector2(0, pathBarPos.y + 22), 
                         Vector2((float)screenWidth, pathBarPos.y + 22), 
                         Color(0.8f, 0.8f, 0.8f), 1.0f);
        
        Vector2 assetStart = projectPos + Vector2(10, 50);
        int assetsPerRow = (int)(projectSize.x / 120); // Increased width for better preview
        
        for (size_t i = 0; i < assets.size(); i++)
        {
            int row = (int)(i / assetsPerRow);
            int col = (int)(i % assetsPerRow);
            Vector2 assetPos = assetStart + Vector2((float)(col * 120), (float)(row * 100));
            
            // Asset background
            renderer->FillRectangle(assetPos, Vector2(110, 90), Color(0.95f, 0.95f, 0.95f));
            renderer->DrawRectangle(assetPos, Vector2(110, 90), Color(0.7f, 0.7f, 0.7f), 1.0f);
            
            // Preview area
            Vector2 previewPos = assetPos + Vector2(5, 5);
            Vector2 previewSize(100, 60);
            
            if (assets[i].type == "image")
            {
                // Draw image preview placeholder
                renderer->FillRectangle(previewPos, previewSize, Color(0.2f, 0.7f, 0.2f));
                renderer->DrawText(L"IMG", previewPos + Vector2(30, 25), Color::White, L"Arial", 10.0f);
            }
            else if (assets[i].type == "audio")
            {
                // Draw audio icon
                renderer->FillRectangle(previewPos + Vector2(30, 15), Vector2(40, 30), Color(0.2f, 0.6f, 0.9f));
                renderer->DrawText(L"AUDIO", previewPos + Vector2(30, 25), Color::White, L"Arial", 10.0f);
            }
            else
            {
                // Draw file icon
                renderer->FillRectangle(previewPos + Vector2(35, 20), Vector2(30, 20), Color(0.6f, 0.6f, 0.6f));
                renderer->DrawText(L"FILE", previewPos + Vector2(35, 25), Color::White, L"Arial", 10.0f);
            }
            
            // File name and details
            Vector2 textPos = assetPos + Vector2(4, 68);
            std::wstring assetName = std::wstring(assets[i].name.begin(), assets[i].name.end());
            
            // Truncate long names
            if (assetName.length() > 15)
            {
                assetName = assetName.substr(0, 12) + L"...";
            }
            
            renderer->DrawText(assetName.c_str(), textPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 8.0f);
            
            // Show file size and type
            Vector2 detailPos = assetPos + Vector2(4, 80);
            std::wstring typeStr = std::wstring(assets[i].type.begin(), assets[i].type.end());
            
            try {
                auto fileSize = std::filesystem::file_size(assets[i].path);
                std::wstring sizeStr;
                if (fileSize < 1024)
                    sizeStr = std::to_wstring(fileSize) + L"B";
                else if (fileSize < 1024 * 1024)
                    sizeStr = std::to_wstring(fileSize / 1024) + L"KB";
                else
                    sizeStr = std::to_wstring(fileSize / (1024 * 1024)) + L"MB";
                    
                std::wstring details = typeStr + L" " + sizeStr;
                renderer->DrawText(details.c_str(), detailPos, Color(0.4f, 0.4f, 0.4f), L"Arial", 7.0f);
            }
            catch (...) {
                renderer->DrawText(typeStr.c_str(), detailPos, Color(0.4f, 0.4f, 0.4f), L"Arial", 7.0f);
            }
        }
        
        if (assets.empty())
        {
            Vector2 centerPos = projectPos + projectSize / 2;
            renderer->DrawText(UI::UI_DRAG_ASSETS, centerPos - Vector2(60, 0), Color(0.4f, 0.4f, 0.4f), L"Arial", 10.0f);
            renderer->DrawText(L"Real files are displayed here", centerPos - Vector2(80, -20), Color(0.4f, 0.4f, 0.4f), L"Arial", 9.0f);
        }
    }

    void LightUnityEditor::DrawCreateMenu(Renderer* renderer)
    {
        Vector2 menuSize(120, 80);
        renderer->FillRectangle(createMenuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(createMenuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = createMenuPos + Vector2(8, 8);
        const wchar_t* items[] = {L"Empty Object", L"Sprite", L"Cube", L"Audio Source"};
        
        for (int i = 0; i < 4; i++)
        {
            renderer->DrawText(items[i], itemPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 9.0f);
            itemPos.y += 18;
        }
    }

    void LightUnityEditor::DrawFileMenu(Renderer* renderer)
    {
        Vector2 menuPos(8, 30);
        Vector2 menuSize(150, 140);
        
        renderer->FillRectangle(menuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(menuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = menuPos + Vector2(8, 8);
        const wchar_t* items[] = {
            UI::FILE_NEW_SCENE,
            UI::FILE_OPEN_SCENE,
            UI::FILE_SAVE,
            UI::FILE_SAVE_AS,
            UI::FILE_BUILD_SETTINGS,
            UI::FILE_BUILD_RUN,
            UI::FILE_EXIT
        };
        
        for (int i = 0; i < 7; i++)
        {
            renderer->DrawText(items[i], itemPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 9.0f);
            itemPos.y += 14;
        }
    }

    void LightUnityEditor::DrawEditMenu(Renderer* renderer)
    {
        Vector2 menuPos(88, 30);
        Vector2 menuSize(120, 120);
        
        renderer->FillRectangle(menuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(menuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = menuPos + Vector2(8, 8);
        const wchar_t* items[] = {
            UI::EDIT_UNDO,
            UI::EDIT_REDO,
            L"----",
            UI::EDIT_CUT,
            UI::EDIT_COPY,
            UI::EDIT_PASTE,
            L"----",
            UI::EDIT_DUPLICATE,
            UI::EDIT_DELETE
        };
        
        for (int i = 0; i < 9; i++)
        {
            if (wcscmp(items[i], L"----") == 0)
            {
                renderer->DrawLine(itemPos, itemPos + Vector2(menuSize.x - 16, 0), Color(0.7f, 0.7f, 0.7f), 1.0f);
            }
            else
            {
                renderer->DrawText(items[i], itemPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 9.0f);
            }
            itemPos.y += 14;
        }
    }

    void LightUnityEditor::DrawGameObjectMenu(Renderer* renderer)
    {
        Vector2 menuPos(248, 30);
        Vector2 menuSize(150, 100);
        
        renderer->FillRectangle(menuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(menuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = menuPos + Vector2(8, 8);
        const wchar_t* items[] = {
            UI::GO_CREATE_EMPTY,
            UI::GO_CREATE_CHILD,
            L"----",
            UI::GO_SPRITE,
            UI::GO_CAMERA,
            L"Camera3D",
            L"3D Object",
            UI::GO_AUDIO_SOURCE
        };
        
        for (int i = 0; i < 8; i++)
        {
            if (wcscmp(items[i], L"----") == 0)
            {
                renderer->DrawLine(itemPos, itemPos + Vector2(menuSize.x - 16, 0), Color(0.7f, 0.7f, 0.7f), 1.0f);
            }
            else
            {
                renderer->DrawText(items[i], itemPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 9.0f);
            }
            itemPos.y += 14;
        }
    }

    void LightUnityEditor::DrawComponentMenu(Renderer* renderer)
    {
        Vector2 menuPos(408, 30);
        Vector2 menuSize(140, 80);
        
        renderer->FillRectangle(menuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(menuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = menuPos + Vector2(8, 8);
        const wchar_t* items[] = {
            UI::INSPECTOR_SPRITE_RENDERER,
            L"Audio Source",
            L"Rigidbody",
            L"Collider",
            L"Script"
        };
        
        for (int i = 0; i < 5; i++)
        {
            renderer->DrawText(items[i], itemPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 9.0f);
            itemPos.y += 14;
        }
    }

    void LightUnityEditor::HandleInput(Vector2 mousePos, bool leftClick, bool rightClick)
    {
        if (!isEnabled) return;
        
        // Prevent duplicate click processing
        if (leftClick && lastClickProcessed && mousePos.x == lastClickPos.x && mousePos.y == lastClickPos.y)
        {
            return;
        }
        
        if (leftClick)
        {
            lastClickProcessed = true;
            lastClickPos = mousePos;
            // Check File Menu Item Clicks
            if (showFileMenu && HandleFileMenuClick(mousePos))
            {
                showFileMenu = false;
                return;
            }
            
            // Check Edit Menu Item Clicks
            if (showEditMenu && HandleEditMenuClick(mousePos))
            {
                showEditMenu = false;
                return;
            }
            
            // Check GameObject Menu Item Clicks
            if (showGameObjectMenu && HandleGameObjectMenuClick(mousePos))
            {
                showGameObjectMenu = false;
                return;
            }
            
            // Check Component Menu Item Clicks
            if (showComponentMenu && HandleComponentMenuClick(mousePos))
            {
                showComponentMenu = false;
                return;
            }
            
            // Check Window Menu Item Clicks
            if (showWindowMenu && HandleWindowMenuClick(mousePos))
            {
                showWindowMenu = false;
                return;
            }
            
            // Check Help Menu Item Clicks
            if (showHelpMenu && HandleHelpMenuClick(mousePos))
            {
                showHelpMenu = false;
                return;
            }
            
            showFileMenu = false;
            showEditMenu = false;
            showGameObjectMenu = false;
            showComponentMenu = false;
            showWindowMenu = false;
            showHelpMenu = false;
            
            if (IsPointInRect(mousePos, Vector2(8, 8), Vector2(72, 14)))
                showFileMenu = true;
            else if (IsPointInRect(mousePos, Vector2(88, 8), Vector2(72, 14)))
                showEditMenu = true;
            else if (IsPointInRect(mousePos, Vector2(248, 8), Vector2(72, 14)))
                showGameObjectMenu = true;
            else if (IsPointInRect(mousePos, Vector2(328, 8), Vector2(72, 14)))
                showComponentMenu = true;
            else if (IsPointInRect(mousePos, Vector2(488, 8), Vector2(72, 14)))
                showWindowMenu = true;
            else if (IsPointInRect(mousePos, Vector2(568, 8), Vector2(72, 14)))
                showHelpMenu = true;

            Vector2 playBtnPos((float)screenWidth / 2 - 45, 4);
            if (IsPointInRect(mousePos, playBtnPos, Vector2(28, 22)))
            {
                isPlayMode = !isPlayMode;
                std::cout << (isPlayMode ? "\\uD50C\\uB808\\uC774 \\uBAA8\\uB4DC \\uC2DC\\uC791" : "\\uD50C\\uB808\\uC774 \\uBAA8\\uB4DC \\uC885\\uB8CC") << std::endl;
            }

            Vector2 toolBtnPos(600, 4);
            for (int i = 0; i < 5; i++)
            {
                if (IsPointInRect(mousePos, toolBtnPos + Vector2((float)(i * 30), 0), Vector2(28, 22)))
                {
                    selectedTool = i;
                    break;
                }
            }
            
            if (IsPointInRect(mousePos, hierarchyPos + Vector2(8, 50), hierarchySize - Vector2(16, 55)))
            {
                Vector2 objListPos = hierarchyPos + Vector2(8, 50);
                Vector2 itemPos = objListPos + Vector2(4, 4);
                
                selectedObject = nullptr;
                for (size_t i = 0; i < sceneObjects.size(); i++)
                {
                    if (IsPointInRect(mousePos, itemPos, Vector2(hierarchySize.x - 16, 18)))
                    {
                        SelectObject(sceneObjects[i].get());
                        break;
                    }
                    itemPos.y += 20;
                }
            }
            
            if (IsPointInRect(mousePos, scenePos + Vector2(0, 24), sceneSize - Vector2(0, 24)))
            {
                Vector2 sceneWorldPos = mousePos - scenePos - Vector2(0, 24);
                
                GameObject* clickedObj = nullptr;
                for (auto& obj : sceneObjects)
                {
                    auto transform = obj->GetComponent<Transform>();
                    if (transform)
                    {
                        Vector2 objPos = transform->GetLocalPosition();
                        if (IsPointInRect(sceneWorldPos, objPos, Vector2(64, 64)))
                        {
                            clickedObj = obj.get();
                            break;
                        }
                    }
                }
                
                if (clickedObj)
                {
                    SelectObject(clickedObj);
                    if (selectedTool == 1)
                    {
                        StartDrag(clickedObj, mousePos);
                    }
                }
                else
                {
                    selectedObject = nullptr;
                }
            }
            
            if (IsPointInRect(mousePos, projectPos + Vector2(0, 42), projectSize - Vector2(0, 42)))
            {
                Vector2 assetStart = projectPos + Vector2(10, 50);
                int assetsPerRow = (int)(projectSize.x / 120);
                
                for (size_t i = 0; i < assets.size(); i++)
                {
                    int row = (int)(i / assetsPerRow);
                    int col = (int)(i % assetsPerRow);
                    Vector2 assetPos = assetStart + Vector2((float)(col * 120), (float)(row * 100));
                    
                    if (IsPointInRect(mousePos, assetPos, Vector2(110, 90)))
                    {
                        if (assets[i].type == "image")
                        {
                            Vector2 sceneWorldPos = Vector2(100, 100);
                            auto sprite = CreateSpriteGameObject("Sprite_" + assets[i].name, sceneWorldPos);
                            
                            // Set the actual texture to the sprite
                            auto spriteRenderer = sprite->GetComponent<SpriteRenderer>();
                            // Note: SpriteRenderer texture setting would be implemented here
                            // when the SetTexture method is available
                        }
                        else if (assets[i].type == "audio")
                        {
                            Vector2 audioPos = Vector2(150, 150);
                            CreateAudioSourceObject(audioPos);
                        }
                        
                        std::cout << "Asset used: " << assets[i].name << " (" << assets[i].type << ")" << std::endl;
                        return; // Exit immediately after creating one object
                    }
                }
            }
        }
        
        if (rightClick)
        {
            if (IsPointInRect(mousePos, scenePos + Vector2(0, 24), sceneSize - Vector2(0, 24)))
            {
                showCreateMenu = true;
                createMenuPos = mousePos;
            }
            else if (IsPointInRect(mousePos, hierarchyPos + Vector2(8, 50), hierarchySize - Vector2(16, 55)))
            {
                showCreateMenu = true;
                createMenuPos = mousePos;
            }
            else
            {
                showCreateMenu = false;
            }
        }
        else
        {
            lastClickProcessed = false;
        }
        
        if (isDragging && draggedObject)
        {
            UpdateDrag(mousePos);
        }
    }

    void LightUnityEditor::HandleKeyInput(int keyCode, bool shift, bool ctrl)
    {
        if (!isEnabled) return;
        
        if (keyCode == VK_DELETE && selectedObject)
        {
            DeleteSelected();
        }
        else if (keyCode == VK_F2 && selectedObject)
        {
            StartTextEdit("name", selectedObject->GetName());
        }
        else if (ctrl && keyCode == 'D' && selectedObject)
        {
            auto transform = selectedObject->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetLocalPosition();
                CreateEmptyGameObject(selectedObject->GetName() + "_Copy", pos + Vector2(20, 20));
            }
        }
        else if (ctrl && keyCode == 'S')
        {
            SaveScene("scene.json");
        }
        else if (ctrl && keyCode == 'O')
        {
            LoadScene("scene.json");
        }
    }

    void LightUnityEditor::HandleChar(wchar_t ch)
    {
        if (isEditingText)
        {
            if (ch == L'\\r' || ch == L'\\n')
            {
                EndTextEdit(true);
            }
            else if (ch == 27)
            {
                EndTextEdit(false);
            }
            else if (ch == L'\\b')
            {
                if (!tempEditValue.empty())
                {
                    tempEditValue.pop_back();
                }
            }
            else if (ch >= 32)
            {
                tempEditValue += (char)ch;
            }
        }
    }

    void LightUnityEditor::StartTextEdit(const std::string& field, const std::string& currentValue)
    {
        isEditingText = true;
        editingField = field;
        tempEditValue = currentValue;
    }

    void LightUnityEditor::EndTextEdit(bool apply)
    {
        if (apply && selectedObject)
        {
            if (editingField == "name")
            {
                selectedObject->SetName(tempEditValue);
            }
            else if (editingField.find("pos") == 0)
            {
                auto transform = selectedObject->GetComponent<Transform>();
                if (transform)
                {
                    Vector2 pos = transform->GetLocalPosition();
                    
                    try 
                    {
                        float value = std::stof(tempEditValue);
                        
                        if (editingField == "posX")
                        {
                            transform->SetLocalPosition(Vector2(value, pos.y));
                        }
                        else if (editingField == "posY")
                        {
                            transform->SetLocalPosition(Vector2(pos.x, value));
                        }
                    }
                    catch (...)
                    {
                        // \\uC798\\uBABB\\uB41C \\uC785\\uB825\\uC740 \\uBB34\\uC2DC
                    }
                }
            }
        }

        isEditingText = false;
        editingField = "";
        tempEditValue = "";
    }

    GameObject* LightUnityEditor::CreateEmptyGameObject(const std::string& name, Vector2 position)
    {
        auto gameObject = std::make_unique<GameObject>(name);
        
        auto transform = gameObject->AddComponent<Transform>();
        transform->SetLocalPosition(position);
        
        GameObject* objPtr = gameObject.get();
        
        // Add to editor's local list
        sceneObjects.push_back(std::move(gameObject));
        
        SelectObject(objPtr);
        std::cout << "Empty Object Created: " << name << std::endl;
        
        return objPtr;
    }

    GameObject* LightUnityEditor::CreateSpriteGameObject(const std::string& name, Vector2 position)
    {
        auto gameObject = CreateEmptyGameObject(name, position);
        
        auto spriteRenderer = gameObject->AddComponent<SpriteRenderer>();
        
        std::cout << "Sprite Object Created: " << name << std::endl;
        
        return gameObject;
    }

    void LightUnityEditor::SelectObject(GameObject* obj)
    {
        selectedObject = obj;
        if (obj)
        {
            std::cout << "\\uC624\\uBE0C\\uC81D\\uD2B8 \\uC120\\uD0DD: " << obj->GetName() << std::endl;
        }
    }

    void LightUnityEditor::DeleteSelected()
    {
        if (!selectedObject) return;
        
        std::string name = selectedObject->GetName();
        
        auto it = std::find_if(sceneObjects.begin(), sceneObjects.end(),
            [this](const std::unique_ptr<GameObject>& obj) 
            {
                return obj.get() == selectedObject;
            });
            
        if (it != sceneObjects.end())
        {
            sceneObjects.erase(it);
            selectedObject = nullptr;
            std::cout << "\\uC624\\uBE0C\\uC81D\\uD2B8 \\uC0AD\\uC81C: " << name << std::endl;
        }
    }

    void LightUnityEditor::StartDrag(GameObject* obj, Vector2 mousePos)
    {
        if (!obj) return;
        
        auto transform = obj->GetComponent<Transform>();
        if (transform)
        {
            isDragging = true;
            draggedObject = obj;
            dragOffset = mousePos - transform->GetLocalPosition();
        }
    }

    void LightUnityEditor::UpdateDrag(Vector2 mousePos)
    {
        if (!isDragging || !draggedObject) return;
        
        auto transform = draggedObject->GetComponent<Transform>();
        if (transform)
        {
            Vector2 newPos = mousePos - dragOffset;
            newPos = newPos - scenePos - Vector2(0, 24);
            transform->SetLocalPosition(newPos);
        }
    }

    void LightUnityEditor::EndDrag()
    {
        isDragging = false;
        draggedObject = nullptr;
        dragOffset = Vector2(0, 0);
    }

    void LightUnityEditor::HandleDragDrop(const std::wstring& filePath)
    {
        std::string path(filePath.begin(), filePath.end());
        std::string ext = path.substr(path.find_last_of("."));
        
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
        {
            LoadImageFile(path);
        }
        else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg")
        {
            LoadAudioFile(path);
        }
    }

    void LightUnityEditor::LoadImageFile(const std::string& filePath)
    {
        AssetItem item;
        item.name = std::filesystem::path(filePath).filename().string();
        item.path = filePath;
        item.type = "image";
        item.texture = TextureManager::GetInstance().LoadTexture(filePath);
        
        assets.push_back(item);
        std::cout << "\\uC774\\uBBF8\\uC9C0 \\uD30C\\uC77C \\uB85C\\uB4DC: " << item.name << std::endl;
    }

    void LightUnityEditor::LoadAudioFile(const std::string& filePath)
    {
        AssetItem item;
        item.name = std::filesystem::path(filePath).filename().string();
        item.path = filePath;
        item.type = "audio";
        item.audio = AudioManager::GetInstance().LoadAudioClip(filePath);
        
        assets.push_back(item);
        std::cout << "\\uC624\\uB514\\uC624 \\uD30C\\uC77C \\uB85C\\uB4DC: " << item.name << std::endl;
    }

    void LightUnityEditor::RefreshAssets()
    {
        assets.clear();
        
        if (std::filesystem::exists(projectPath))
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath))
            {
                if (entry.is_regular_file())
                {
                    AssetItem item;
                    item.name = entry.path().filename().string();
                    item.path = entry.path().string();
                    
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
                    {
                        item.type = "image";
                    }
                    else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg")
                    {
                        item.type = "audio";
                    }
                    else
                    {
                        item.type = "unknown";
                    }
                    
                    assets.push_back(item);
                }
            }
        }
    }

    void LightUnityEditor::SaveScene(const std::string& fileName)
    {
        std::cout << "\\uC52C \\uC800\\uC7A5: " << fileName << std::endl;
    }

    void LightUnityEditor::LoadScene(const std::string& fileName)
    {
        std::cout << "\\uC52C \\uB85C\\uB4DC: " << fileName << std::endl;
    }

    bool LightUnityEditor::HandleFileMenuClick(Vector2 mousePos)
    {
        Vector2 menuPos(8, 30);
        Vector2 itemPos = menuPos + Vector2(8, 8);
        
        for (int i = 0; i < 7; i++)
        {
            if (IsPointInRect(mousePos, itemPos, Vector2(134, 14)))
            {
                switch (i)
                {
                case 0: // New Scene
                    CreateNewScene();
                    break;
                case 1: // Open Scene
                    OpenSceneDialog();
                    break;
                case 2: // Save
                    SaveCurrentScene();
                    break;
                case 3: // Save As
                    SaveAsDialog();
                    break;
                case 4: // Build Settings
                    ShowBuildSettings();
                    break;
                case 5: // Build and Run
                    BuildAndRun();
                    break;
                case 6: // Exit
                    ExitApplication();
                    break;
                }
                return true;
            }
            itemPos.y += 14;
        }
        return false;
    }
    
    bool LightUnityEditor::HandleEditMenuClick(Vector2 mousePos)
    {
        Vector2 menuPos(88, 30);
        Vector2 itemPos = menuPos + Vector2(8, 8);
        
        int menuIndex = 0;
        for (int i = 0; i < 9; i++)
        {
            if (i == 2 || i == 6) // separator lines
            {
                itemPos.y += 14;
                continue;
            }
            
            if (IsPointInRect(mousePos, itemPos, Vector2(104, 14)))
            {
                switch (menuIndex)
                {
                case 0: UndoAction(); break;
                case 1: RedoAction(); break;
                case 2: CutSelected(); break;
                case 3: CopySelected(); break;
                case 4: PasteObject(); break;
                case 5: DuplicateSelected(); break;
                case 6: DeleteSelected(); break;
                }
                return true;
            }
            itemPos.y += 14;
            menuIndex++;
        }
        return false;
    }
    
    bool LightUnityEditor::HandleGameObjectMenuClick(Vector2 mousePos)
    {
        Vector2 menuPos(248, 30);
        Vector2 itemPos = menuPos + Vector2(8, 8);
        
        int menuIndex = 0;
        for (int i = 0; i < 6; i++)
        {
            if (i == 2) // separator line
            {
                itemPos.y += 14;
                continue;
            }
            
            if (IsPointInRect(mousePos, itemPos, Vector2(134, 14)))
            {
                Vector2 createPos(400, 300);
                switch (menuIndex)
                {
                case 0: CreateEmptyGameObject("Empty Object", createPos); break;
                case 1: CreateChildObject(); break;
                case 2: CreateSpriteGameObject("Sprite", createPos); break;
                case 3: CreateCameraObject(createPos); break;
                case 4: CreateAudioSourceObject(createPos); break;
                }
                return true;
            }
            itemPos.y += 14;
            menuIndex++;
        }
        return false;
    }
    
    bool LightUnityEditor::HandleComponentMenuClick(Vector2 mousePos)
    {
        Vector2 menuPos(408, 30);
        Vector2 itemPos = menuPos + Vector2(8, 8);
        
        if (!selectedObject) return false;
        
        for (int i = 0; i < 5; i++)
        {
            if (IsPointInRect(mousePos, itemPos, Vector2(124, 14)))
            {
                switch (i)
                {
                case 0: AddSpriteRenderer(); break;
                case 1: AddAudioSource(); break;
                case 2: AddRigidbody(); break;
                case 3: AddCollider(); break;
                case 4: AddScript(); break;
                }
                return true;
            }
            itemPos.y += 14;
        }
        return false;
    }
    
    void LightUnityEditor::DrawWindowMenu(Renderer* renderer)
    {
        Vector2 menuPos(488, 30);
        Vector2 menuSize(160, 100);
        
        renderer->FillRectangle(menuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(menuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = menuPos + Vector2(8, 8);
        const wchar_t* items[] = {
            L"Hierarchy",
            L"Scene", 
            L"Inspector",
            L"Project",
            L"Console"
        };
        
        bool* panelStates[] = {
            &showHierarchyPanel,
            &showScenePanel,
            &showInspectorPanel,
            &showProjectPanel,
            &showConsolePanel
        };
        
        for (int i = 0; i < 5; i++)
        {
            Color textColor = *panelStates[i] ? Color(0.1f, 0.6f, 0.1f) : Color(0.05f, 0.05f, 0.05f);
            std::wstring itemText = std::wstring(L"✓ ") + items[i];
            if (!*panelStates[i])
                itemText = std::wstring(L"  ") + items[i];
                
            renderer->DrawText(itemText.c_str(), itemPos, textColor, L"Arial", 9.0f);
            itemPos.y += 16;
        }
    }
    
    void LightUnityEditor::DrawHelpMenu(Renderer* renderer)
    {
        Vector2 menuPos(568, 30);
        Vector2 menuSize(140, 60);
        
        renderer->FillRectangle(menuPos, menuSize, Color(0.92f, 0.92f, 0.92f));
        renderer->DrawRectangle(menuPos, menuSize, Color(0.7f, 0.7f, 0.7f), 1.0f);
        
        Vector2 itemPos = menuPos + Vector2(8, 8);
        const wchar_t* items[] = {
            L"About AronEngine",
            L"Documentation",
            L"Report Bug"
        };
        
        for (int i = 0; i < 3; i++)
        {
            renderer->DrawText(items[i], itemPos, Color(0.05f, 0.05f, 0.05f), L"Arial", 9.0f);
            itemPos.y += 16;
        }
    }
    
    bool LightUnityEditor::HandleWindowMenuClick(Vector2 mousePos)
    {
        Vector2 menuPos(488, 30);
        Vector2 itemPos = menuPos + Vector2(8, 8);
        
        for (int i = 0; i < 5; i++)
        {
            if (IsPointInRect(mousePos, itemPos, Vector2(144, 16)))
            {
                switch (i)
                {
                case 0: ToggleHierarchyPanel(); break;
                case 1: ToggleScenePanel(); break;
                case 2: ToggleInspectorPanel(); break;
                case 3: ToggleProjectPanel(); break;
                case 4: ToggleConsolePanel(); break;
                }
                return true;
            }
            itemPos.y += 16;
        }
        return false;
    }
    
    bool LightUnityEditor::HandleHelpMenuClick(Vector2 mousePos)
    {
        Vector2 menuPos(568, 30);
        Vector2 itemPos = menuPos + Vector2(8, 8);
        
        for (int i = 0; i < 3; i++)
        {
            if (IsPointInRect(mousePos, itemPos, Vector2(124, 16)))
            {
                switch (i)
                {
                case 0: ShowAboutDialog(); break;
                case 1: ShowDocumentation(); break;
                case 2: 
                    ShellExecute(0, L"open", L"https://github.com/yourusername/AronEngine/issues", 0, 0, SW_SHOWNORMAL);
                    break;
                }
                return true;
            }
            itemPos.y += 16;
        }
        return false;
    }
    
    void LightUnityEditor::ToggleHierarchyPanel()
    {
        showHierarchyPanel = !showHierarchyPanel;
        CalculateLayout();
        std::cout << "Hierarchy Panel " << (showHierarchyPanel ? "Opened" : "Closed") << std::endl;
    }
    
    void LightUnityEditor::ToggleScenePanel()
    {
        showScenePanel = !showScenePanel;
        CalculateLayout();
        std::cout << "Scene Panel " << (showScenePanel ? "Opened" : "Closed") << std::endl;
    }
    
    void LightUnityEditor::ToggleInspectorPanel()
    {
        showInspectorPanel = !showInspectorPanel;
        CalculateLayout();
        std::cout << "Inspector Panel " << (showInspectorPanel ? "Opened" : "Closed") << std::endl;
    }
    
    void LightUnityEditor::ToggleProjectPanel()
    {
        showProjectPanel = !showProjectPanel;
        CalculateLayout();
        std::cout << "Project Panel " << (showProjectPanel ? "Opened" : "Closed") << std::endl;
    }
    
    void LightUnityEditor::ToggleConsolePanel()
    {
        showConsolePanel = !showConsolePanel;
        CalculateLayout();
        std::cout << "Console Panel " << (showConsolePanel ? "Opened" : "Closed") << std::endl;
    }
    
    void LightUnityEditor::ShowAboutDialog()
    {
        MessageBoxW(NULL, 
                   L"AronEngine v1.0\\n\\n"
                   L"Unity 스타일 게임 엔진\\n\\n"
                   L"기능:\\n"
                   L"- 컴포넌트 기반 아키텍처\\n"
                   L"- Direct2D 렌더링\\n"
                   L"- FMOD 오디오 시스템\\n"
                   L"- 이팩트 시스템\\n"
                   L"- AABB 컬링\\n\\n"
                   L"Made with Love",
                   L"About AronEngine",
                   MB_OK | MB_ICONINFORMATION);
    }
    
    void LightUnityEditor::ShowDocumentation()
    {
        ShellExecute(0, L"open", L"https://github.com/yourusername/AronEngine/wiki", 0, 0, SW_SHOWNORMAL);
        std::cout << "Documentation opened" << std::endl;
    }
    
    void LightUnityEditor::DrawConsole(Renderer* renderer)
    {
        if (!showConsolePanel) return;
        
        renderer->FillRectangle(consolePos, consoleSize, Color(0.15f, 0.15f, 0.15f));
        renderer->DrawLine(Vector2(0, consolePos.y), Vector2((float)screenWidth, consolePos.y), Color(0.8f, 0.8f, 0.8f), 1.0f);
        
        Vector2 headerPos = consolePos;
        Vector2 headerSize(consoleSize.x, 20.0f);
        renderer->FillRectangle(headerPos, headerSize, Color(0.25f, 0.25f, 0.25f));
        renderer->DrawText(L"Console", headerPos + Vector2(8, 4), Color(0.9f, 0.9f, 0.9f), L"Arial", 10.0f);
        
        Vector2 clearBtnPos = headerPos + Vector2(consoleSize.x - 60, 2);
        Vector2 clearBtnSize(50, 16);
        renderer->FillRectangle(clearBtnPos, clearBtnSize, Color(0.6f, 0.3f, 0.3f));
        renderer->DrawText(L"Clear", clearBtnPos + Vector2(12, 2), Color::White, L"Arial", 8.0f);
        
        Vector2 logPos = consolePos + Vector2(8, 25);
        Vector2 logSize = consoleSize - Vector2(16, 30);
        
        renderer->FillRectangle(logPos, logSize, Color(0.1f, 0.1f, 0.1f));
        
        Vector2 textPos = logPos + Vector2(4, 4);
        renderer->DrawText(L"AronEngine Console - Ready", textPos, Color(0.8f, 0.8f, 0.8f), L"Consolas", 9.0f);
        textPos.y += 15;
        renderer->DrawText(L"Game Engine Initialized Successfully.", textPos, Color(0.7f, 0.9f, 0.7f), L"Consolas", 9.0f);
        textPos.y += 15;
        renderer->DrawText(L"Console logs will be displayed here.", textPos, Color(0.6f, 0.6f, 0.6f), L"Consolas", 9.0f);
    }
    
    void LightUnityEditor::CreateNewScene()
    {
        sceneObjects.clear();
        selectedObject = nullptr;
        
        // Create new scene in SceneManager
        auto sceneManager = GameEngine::GetInstance().GetSceneManager();
        if (sceneManager)
        {
            std::string sceneName = "New Scene " + std::to_string(sceneManager->GetSceneCount() + 1);
            auto newScene = sceneManager->CreateScene(sceneName);
            sceneManager->LoadScene(sceneName);
        }
        
        std::cout << "New scene created" << std::endl;
    }
    
    void LightUnityEditor::OpenSceneDialog()
    {
        OPENFILENAME ofn;
        wchar_t szFile[260] = {0};
        
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Scene Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        
        if (GetOpenFileName(&ofn))
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
            std::string fileName(size, 0);
            WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &fileName[0], size, NULL, NULL);
            fileName.resize(size - 1);
            LoadSceneFromFile(fileName);
        }
    }
    
    void LightUnityEditor::SaveCurrentScene()
    {
        std::string fileName = "scene.json";
        SaveSceneToFile(fileName);
        std::cout << "Scene saved: " << fileName << std::endl;
    }
    
    void LightUnityEditor::SaveAsDialog()
    {
        OPENFILENAME ofn;
        wchar_t szFile[260] = {0};
        
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Scene Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        
        if (GetSaveFileName(&ofn))
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
            std::string fileName(size, 0);
            WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &fileName[0], size, NULL, NULL);
            fileName.resize(size - 1);
            SaveSceneToFile(fileName);
            std::cout << "Scene saved as: " << fileName << std::endl;
        }
    }
    
    void LightUnityEditor::ShowBuildSettings()
    {
        std::cout << "Build settings opened" << std::endl;
    }
    
    void LightUnityEditor::BuildAndRun()
    {
        std::cout << "Build and run started" << std::endl;
    }
    
    void LightUnityEditor::ExitApplication()
    {
        PostQuitMessage(0);
    }
    
    void LightUnityEditor::UndoAction()
    {
        std::cout << "Undo action" << std::endl;
    }
    
    void LightUnityEditor::RedoAction()
    {
        std::cout << "Redo action" << std::endl;
    }
    
    void LightUnityEditor::CutSelected()
    {
        if (selectedObject)
        {
            CopySelected();
            DeleteSelected();
        }
    }
    
    void LightUnityEditor::CopySelected()
    {
        if (selectedObject)
        {
            std::cout << "Object copied: " << selectedObject->GetName() << std::endl;
        }
    }
    
    void LightUnityEditor::PasteObject()
    {
        std::cout << "Object pasted" << std::endl;
    }
    
    void LightUnityEditor::DuplicateSelected()
    {
        if (!selectedObject) return;
        
        auto transform = selectedObject->GetComponent<Transform>();
        if (transform)
        {
            Vector2 pos = transform->GetLocalPosition();
            CreateEmptyGameObject(selectedObject->GetName() + "_Copy", pos + Vector2(20, 20));
        }
    }
    
    void LightUnityEditor::CreateChildObject()
    {
        if (selectedObject)
        {
            auto transform = selectedObject->GetComponent<Transform>();
            Vector2 pos = transform ? transform->GetLocalPosition() : Vector2(0, 0);
            CreateEmptyGameObject("Child Object", pos + Vector2(0, 30));
        }
        else
        {
            CreateEmptyGameObject("Child Object", Vector2(300, 300));
        }
    }
    
    void LightUnityEditor::CreateCameraObject(Vector2 position)
    {
        auto camera = CreateEmptyGameObject("Camera", position);
        std::cout << "Camera object created" << std::endl;
    }
    
    void LightUnityEditor::CreateAudioSourceObject(Vector2 position)
    {
        auto audioObj = CreateEmptyGameObject("Audio Source", position);
        audioObj->AddComponent<AudioSource>();
        std::cout << "Audio Source Object Created" << std::endl;
    }
    
    void LightUnityEditor::AddSpriteRenderer()
    {
        if (selectedObject && !selectedObject->GetComponent<SpriteRenderer>())
        {
            selectedObject->AddComponent<SpriteRenderer>();
            std::cout << "Sprite renderer component added" << std::endl;
        }
    }
    
    void LightUnityEditor::AddAudioSource()
    {
        if (selectedObject && !selectedObject->GetComponent<AudioSource>())
        {
            selectedObject->AddComponent<AudioSource>();
            std::cout << "Audio source component added" << std::endl;
        }
    }
    
    void LightUnityEditor::AddRigidbody()
    {
        std::cout << "Rigidbody component added" << std::endl;
    }
    
    void LightUnityEditor::AddCollider()
    {
        std::cout << "Collider component added" << std::endl;
    }
    
    void LightUnityEditor::AddScript()
    {
        std::cout << "Script component added" << std::endl;
    }
    
    void LightUnityEditor::SaveSceneToFile(const std::string& fileName)
    {
        std::ofstream file(fileName);
        if (file.is_open())
        {
            file << "{\n";
            file << "  \"scene\": {\n";
            file << "    \"name\": \"Scene\",\n";
            file << "    \"objects\": [\n";
            
            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                auto& obj = sceneObjects[i];
                auto transform = obj->GetComponent<Transform>();
                Vector2 pos = transform ? transform->GetLocalPosition() : Vector2(0, 0);
                
                file << "      {\n";
                file << "        \"name\": \"" << obj->GetName() << "\",\n";
                file << "        \"position\": [" << pos.x << ", " << pos.y << "]\n";
                file << "      }";
                if (i < sceneObjects.size() - 1) file << ",";
                file << "\n";
            }
            
            file << "    ]\n";
            file << "  }\n";
            file << "}\n";
            file.close();
        }
    }
    
    void LightUnityEditor::LoadSceneFromFile(const std::string& fileName)
    {
        std::ifstream file(fileName);
        if (file.is_open())
        {
            sceneObjects.clear();
            selectedObject = nullptr;
            
            // Parse JSON and recreate objects
            std::string line;
            bool inObjectsArray = false;
            
            while (std::getline(file, line))
            {
                if (line.find("\"objects\"") != std::string::npos)
                {
                    inObjectsArray = true;
                    continue;
                }
                
                if (inObjectsArray && line.find("\"name\"") != std::string::npos)
                {
                    // Extract object name
                    size_t nameStart = line.find("\"") + 1;
                    size_t nameEnd = line.find("\"", nameStart + 1);
                    if (nameStart != std::string::npos && nameEnd != std::string::npos)
                    {
                        std::string objName = line.substr(nameStart, nameEnd - nameStart);
                        CreateEmptyGameObject(objName, Vector2(200, 200));
                    }
                }
            }
            
            file.close();
            std::cout << "Scene loaded: " << fileName << std::endl;
        }
    }
    
    bool LightUnityEditor::IsPointInRect(Vector2 point, Vector2 pos, Vector2 size)
    {
        return point.x >= pos.x && point.x <= pos.x + size.x &&
               point.y >= pos.y && point.y <= pos.y + size.y;
    }
}