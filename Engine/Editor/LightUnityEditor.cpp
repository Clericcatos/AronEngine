#include "LightUnityEditor.h"
#include "KoreanUI.h"
#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Core/TextureManager.h"
#include "Components/Transform.h"
#include "Components/SpriteRenderer.h"
#include "Components/AudioSource.h"
#include "Resources/AudioClip.h"
#include "Resources/Texture2D.h"
#include "Managers/AudioManager.h"
#include "Utils/Color.h"
#include <shellapi.h>
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
        , isEditingText(false)
        , editingField("")
        , tempEditValue("")
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
        const float hierarchyWidth = 250.0f;
        const float inspectorWidth = 300.0f;
        const float projectHeight = 200.0f;
        
        toolbarPos = Vector2(0, 0);
        toolbarSize = Vector2((float)screenWidth, menuHeight);
        
        hierarchyPos = Vector2(0, menuHeight);
        hierarchySize = Vector2(hierarchyWidth, (float)screenHeight - menuHeight - projectHeight);
        
        scenePos = Vector2(hierarchyWidth, menuHeight);
        sceneSize = Vector2((float)screenWidth - hierarchyWidth - inspectorWidth, (float)screenHeight - menuHeight - projectHeight);
        
        inspectorPos = Vector2((float)screenWidth - inspectorWidth, menuHeight);
        inspectorSize = Vector2(inspectorWidth, (float)screenHeight - menuHeight - projectHeight);
        
        projectPos = Vector2(0, (float)screenHeight - projectHeight);
        projectSize = Vector2((float)screenWidth, projectHeight);
    }

    void LightUnityEditor::Render(Renderer* renderer)
    {
        if (!isEnabled) return;
        
        DrawToolbar(renderer);
        DrawHierarchy(renderer);
        DrawSceneView(renderer);
        DrawInspector(renderer);
        DrawProject(renderer);
        
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
    }

    void LightUnityEditor::DrawInspector(Renderer* renderer)
    {
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
            
            contentPos.y += 10;
            renderer->DrawText(UI::INSPECTOR_ADD_COMPONENT, contentPos, Color(0.3f, 0.3f, 0.8f), L"Arial", 10.0f);
        }
    }

    void LightUnityEditor::DrawProject(Renderer* renderer)
    {
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
        int assetsPerRow = (int)(projectSize.x / 80);
        
        for (size_t i = 0; i < assets.size(); i++)
        {
            int row = (int)(i / assetsPerRow);
            int col = (int)(i % assetsPerRow);
            Vector2 assetPos = assetStart + Vector2((float)(col * 80), (float)(row * 80));
            
            renderer->FillRectangle(assetPos, Vector2(60, 60), Color(0.95f, 0.95f, 0.95f));
            renderer->DrawRectangle(assetPos, Vector2(60, 60), Color(0.7f, 0.7f, 0.7f), 1.0f);
            
            Vector2 textPos = assetPos + Vector2(4, 64);
            std::wstring assetName = std::wstring(assets[i].name.begin(), assets[i].name.end());
            renderer->DrawText(assetName.c_str(), textPos, Color(0.1f, 0.1f, 0.1f), L"Arial", 8.0f);
        }
        
        if (assets.empty())
        {
            Vector2 centerPos = projectPos + projectSize / 2;
            renderer->DrawText(UI::UI_DRAG_ASSETS, centerPos - Vector2(60, 0), Color(0.4f, 0.4f, 0.4f), L"Arial", 10.0f);
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
            UI::GO_AUDIO_SOURCE
        };
        
        for (int i = 0; i < 6; i++)
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
        
        if (leftClick)
        {
            showFileMenu = false;
            showEditMenu = false;
            showGameObjectMenu = false;
            showComponentMenu = false;
            
            if (IsPointInRect(mousePos, Vector2(8, 8), Vector2(72, 14)))
                showFileMenu = true;
            else if (IsPointInRect(mousePos, Vector2(88, 8), Vector2(72, 14)))
                showEditMenu = true;
            else if (IsPointInRect(mousePos, Vector2(248, 8), Vector2(72, 14)))
                showGameObjectMenu = true;
            else if (IsPointInRect(mousePos, Vector2(328, 8), Vector2(72, 14)))
                showComponentMenu = true;

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
                int assetsPerRow = (int)(projectSize.x / 80);
                
                for (size_t i = 0; i < assets.size(); i++)
                {
                    int row = (int)(i / assetsPerRow);
                    int col = (int)(i % assetsPerRow);
                    Vector2 assetPos = assetStart + Vector2((float)(col * 80), (float)(row * 80));
                    
                    if (IsPointInRect(mousePos, assetPos, Vector2(60, 60)))
                    {
                        if (assets[i].type == "image")
                        {
                            Vector2 sceneWorldPos = Vector2(100, 100);
                            CreateSpriteGameObject("Sprite_" + assets[i].name, sceneWorldPos);
                        }
                        break;
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
        sceneObjects.push_back(std::move(gameObject));
        
        SelectObject(objPtr);
        std::cout << "\\uBE48 \\uC624\\uBE0C\\uC81D\\uD2B8 \\uC0DD\\uC131: " << name << std::endl;
        
        return objPtr;
    }

    GameObject* LightUnityEditor::CreateSpriteGameObject(const std::string& name, Vector2 position)
    {
        auto gameObject = CreateEmptyGameObject(name, position);
        
        auto spriteRenderer = gameObject->AddComponent<SpriteRenderer>();
        
        std::cout << "\\uC2A4\\uD504\\uB77C\\uC774\\uD2B8 \\uC624\\uBE0C\\uC81D\\uD2B8 \\uC0DD\\uC131: " << name << std::endl;
        
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

    bool LightUnityEditor::IsPointInRect(Vector2 point, Vector2 pos, Vector2 size)
    {
        return point.x >= pos.x && point.x <= pos.x + size.x &&
               point.y >= pos.y && point.y <= pos.y + size.y;
    }
}