#include "framework.h"
#include "RealUnityEditor.h"
#include "../Core/Renderer.h"
#include "../Core/GameObject.h"
#include "../Core/TextureManager.h"
#include "../Components/Transform.h"
#include "../Components/SpriteRenderer.h"
#include "../Resources/Texture2D.h"
#include "../Resources/AudioClip.h"
#include "../Managers/AudioManager.h"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include "../Components/AudioSource.h"
#include "KoreanUI.h"

namespace AronEngine
{
    RealUnityEditor::RealUnityEditor()
        : isEnabled(true)
        , selectedObject(nullptr)
        , screenWidth(1920)
        , screenHeight(1080)
        , showCreateMenu(false)
        , isDragging(false)
        , draggedObject(nullptr)
        , projectPath("./Assets")
        , selectedIndex(-1)
        , isPlayMode(false)
        , selectedTool(1) // Default to Move tool
        , searchQuery("")
        , showFileMenu(false)
        , showEditMenu(false)
        , showGameObjectMenu(false)
        , showComponentMenu(false)
    {
    }

    RealUnityEditor::~RealUnityEditor()
    {
        Shutdown();
    }

    bool RealUnityEditor::Initialize(int width, int height)
    {
        screenWidth = width;
        screenHeight = height;
        CalculateLayout();
        
        // Create Assets folder
        std::filesystem::create_directories(projectPath);
        RefreshAssets();
        
        DEBUG_LOG("Real Unity Editor initialized - " + std::to_string(width) + "x" + std::to_string(height));
        return true;
    }

    void RealUnityEditor::CalculateLayout()
    {
        // Unity's actual layout proportions
        toolbarPos = Vector2(0, 0);
        toolbarSize = Vector2((float)screenWidth, 40);
        
        float contentHeight = (float)screenHeight - 40;
        float bottomPanelHeight = contentHeight * 0.3f; // 30% bottom
        float topPanelHeight = contentHeight - bottomPanelHeight;
        
        // Top panels
        hierarchyPos = Vector2(0, 40);
        hierarchySize = Vector2((float)screenWidth * 0.2f, topPanelHeight); // 20% width
        
        scenePos = Vector2(hierarchySize.x, 40);
        sceneSize = Vector2((float)screenWidth * 0.6f, topPanelHeight); // 60% width
        
        inspectorPos = Vector2((float)screenWidth * 0.8f, 40);
        inspectorSize = Vector2((float)screenWidth * 0.2f, topPanelHeight); // 20% width
        
        // Bottom panel (Project)
        projectPos = Vector2(0, 40 + topPanelHeight);
        projectSize = Vector2((float)screenWidth, bottomPanelHeight);
    }

    void RealUnityEditor::Shutdown()
    {
        sceneObjects.clear();
        assets.clear();
        selectedObject = nullptr;
        DEBUG_LOG("Real Unity Editor shutdown");
    }

    void RealUnityEditor::Update(float deltaTime)
    {
    }

    void RealUnityEditor::Render(Renderer* renderer)
    {
        if (!isEnabled) return;

        DrawToolbar(renderer);
        DrawHierarchy(renderer);
        DrawSceneView(renderer);  
        DrawInspector(renderer);
        DrawProject(renderer);
        
        if (showCreateMenu)
            DrawCreateMenu(renderer);
            
        // Draw dropdown menus
        if (showFileMenu)
            DrawFileMenu(renderer);
        if (showEditMenu)
            DrawEditMenu(renderer);
        if (showGameObjectMenu)
            DrawGameObjectMenu(renderer);
        if (showComponentMenu)
            DrawComponentMenu(renderer);
    }

    void RealUnityEditor::DrawToolbar(Renderer* renderer)
    {
        // Unity toolbar
        renderer->FillRectangle(toolbarPos, toolbarSize, Color(0.22f, 0.22f, 0.22f));
        renderer->DrawLine(Vector2(0, toolbarSize.y), Vector2((float)screenWidth, toolbarSize.y), Color(0.15f, 0.15f, 0.15f), 1.0f);
        
        // Menu bar
        Vector2 menuPos(5, 10);
        const wchar_t* menus[] = {L"File", L"Edit", L"Assets", L"GameObject", L"Component", L"Window", L"Help"};
        
        for (int i = 0; i < 7; i++)
        {
            renderer->DrawText(menus[i], menuPos, Color(0.9f, 0.9f, 0.9f), L"Arial", 12.0f);
            menuPos.x += 60;
        }
        
        // Transform tools
        Vector2 btnPos = toolbarPos + Vector2(480, 8);
        Vector2 btnSize(30, 24);
        
        const wchar_t* tools[] = {L"Q", L"W", L"E", L"R", L"T"};
        const wchar_t* tooltips[] = {L"Hand", L"Move", L"Rotate", L"Scale", L"Rect"};
        
        for (int i = 0; i < 5; i++)
        {
            bool isSelected = (i == selectedTool);
            Color btnColor = isSelected ? Color(0.25f, 0.4f, 0.6f) : Color(0.28f, 0.28f, 0.28f);
            
            renderer->FillRectangle(btnPos, btnSize, btnColor);
            renderer->DrawRectangle(btnPos, btnSize, Color(0.4f, 0.4f, 0.4f), 1.0f);
            renderer->DrawText(tools[i], btnPos + Vector2(11, 6), Color(0.95f, 0.95f, 0.95f), L"Arial", 11.0f);
            btnPos.x += 32;
        }
        
        // Play controls
        btnPos.x = (float)screenWidth / 2 - 60;
        Color playBtnColor = isPlayMode ? Color(0.4f, 0.6f, 0.4f) : Color(0.25f, 0.25f, 0.25f);
        renderer->FillRectangle(btnPos, Vector2(30, 24), playBtnColor);
        renderer->DrawRectangle(btnPos, Vector2(30, 24), Color(0.4f, 0.4f, 0.4f), 1.0f);
        
        // Play icon (triangle)
        Vector2 playCenter = btnPos + Vector2(15, 12);
        Vector2 triangle[3] = {
            playCenter + Vector2(-5, -6),
            playCenter + Vector2(-5, 6),
            playCenter + Vector2(6, 0)
        };
        renderer->FillTriangle(triangle[0], triangle[1], triangle[2], Color(0.7f, 0.7f, 0.7f));
        
        btnPos.x += 32;
        renderer->FillRectangle(btnPos, Vector2(30, 24), Color(0.25f, 0.25f, 0.25f));
        renderer->DrawRectangle(btnPos, Vector2(30, 24), Color(0.4f, 0.4f, 0.4f), 1.0f);
        
        // Pause icon (two bars)
        Vector2 pauseCenter = btnPos + Vector2(15, 12);
        renderer->FillRectangle(pauseCenter + Vector2(-6, -6), Vector2(4, 12), Color(0.7f, 0.7f, 0.7f));
        renderer->FillRectangle(pauseCenter + Vector2(2, -6), Vector2(4, 12), Color(0.7f, 0.7f, 0.7f));
        
        btnPos.x += 32;
        renderer->FillRectangle(btnPos, Vector2(30, 24), Color(0.25f, 0.25f, 0.25f));
        renderer->DrawRectangle(btnPos, Vector2(30, 24), Color(0.4f, 0.4f, 0.4f), 1.0f);
        
        // Step icon (bar + triangle)
        Vector2 stepCenter = btnPos + Vector2(15, 12);
        renderer->FillRectangle(stepCenter + Vector2(-6, -6), Vector2(3, 12), Color(0.7f, 0.7f, 0.7f));
        Vector2 stepTriangle[3] = {
            stepCenter + Vector2(-2, -5),
            stepCenter + Vector2(-2, 5),
            stepCenter + Vector2(5, 0)
        };
        renderer->FillTriangle(stepTriangle[0], stepTriangle[1], stepTriangle[2], Color(0.7f, 0.7f, 0.7f));
    }

    void RealUnityEditor::DrawHierarchy(Renderer* renderer)
    {
        // Unity Hierarchy
        renderer->FillRectangle(hierarchyPos, hierarchySize, Color(0.2f, 0.2f, 0.2f));
        renderer->DrawLine(Vector2(hierarchyPos.x + hierarchySize.x, hierarchyPos.y), 
                         Vector2(hierarchyPos.x + hierarchySize.x, hierarchyPos.y + hierarchySize.y), 
                         Color(0.0f, 0.0f, 0.0f), 2.0f);
        
        // Header
        Vector2 headerSize(hierarchySize.x, 22);
        renderer->FillRectangle(hierarchyPos, headerSize, Color(0.22f, 0.22f, 0.22f));
        renderer->DrawText(L"Hierarchy", hierarchyPos + Vector2(10, 5), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 11.0f);
        renderer->DrawLine(Vector2(hierarchyPos.x, hierarchyPos.y + 22), 
                         Vector2(hierarchyPos.x + hierarchySize.x, hierarchyPos.y + 22), 
                         Color(0.0f, 0.0f, 0.0f), 1.0f);
        
        // Create button
        Vector2 createBtn = hierarchyPos + Vector2(hierarchySize.x - 30, 3);
        Vector2 createBtnSize(16, 16);
        renderer->FillRectangle(createBtn, createBtnSize, Color(0.28f, 0.28f, 0.28f));
        renderer->DrawRectangle(createBtn, createBtnSize, Color(0.4f, 0.4f, 0.4f), 1.0f);
        renderer->DrawText(L"+", createBtn + Vector2(5, 0), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 14.0f);
        
        // Object list
        Vector2 listStart = hierarchyPos + Vector2(0, 25);
        
        if (sceneObjects.empty())
        {
            Vector2 emptyPos = hierarchyPos + hierarchySize / 2 - Vector2(60, 10);
            renderer->DrawText(L"No objects in scene", emptyPos, Color(0.5f, 0.5f, 0.5f), L"Segoe UI", 10.0f);
        }
        else
        {
            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                Vector2 itemPos = listStart + Vector2(0, (float)(i * 20));
                Vector2 itemSize(hierarchySize.x, 20);
                
                bool isSelected = (selectedObject == sceneObjects[i].get());
                
                if (isSelected)
                {
                    renderer->FillRectangle(itemPos, itemSize, Color(0.17f, 0.36f, 0.53f));
                }
                
                // GameObject name
                std::wstring name = std::wstring(sceneObjects[i]->GetName().begin(), sceneObjects[i]->GetName().end());
                // GameObject icon
                Vector2 iconPos = itemPos + Vector2(5, 3);
                renderer->FillRectangle(iconPos, Vector2(14, 14), Color(0.4f, 0.4f, 0.4f));
                renderer->DrawRectangle(iconPos, Vector2(14, 14), Color(0.6f, 0.6f, 0.6f), 1.0f);
                
                Color textColor = isSelected ? Color::White : Color(0.85f, 0.85f, 0.85f);
                renderer->DrawText(name, itemPos + Vector2(25, 3), textColor, L"Segoe UI", 10.0f);
            }
        }
    }

    void RealUnityEditor::DrawSceneView(Renderer* renderer)
    {
        // Unity Scene View
        renderer->FillRectangle(scenePos, sceneSize, Color(0.15f, 0.15f, 0.15f));
        renderer->DrawLine(Vector2(scenePos.x + sceneSize.x, scenePos.y), 
                         Vector2(scenePos.x + sceneSize.x, scenePos.y + sceneSize.y), 
                         Color(0.0f, 0.0f, 0.0f), 2.0f);
        
        // Scene header
        Vector2 headerSize(sceneSize.x, 22);
        renderer->FillRectangle(scenePos, headerSize, Color(0.22f, 0.22f, 0.22f));
        
        // Scene tab
        Vector2 tabPos = scenePos + Vector2(5, 0);
        Vector2 tabSize(60, 22);
        renderer->FillRectangle(tabPos, tabSize, Color(0.25f, 0.25f, 0.25f));
        renderer->DrawText(L"Scene", tabPos + Vector2(15, 5), Color(0.95f, 0.95f, 0.95f), L"Segoe UI", 10.0f);
        
        // Game tab
        tabPos.x += 60;
        renderer->DrawText(L"Game", tabPos + Vector2(15, 5), Color(0.6f, 0.6f, 0.6f), L"Segoe UI", 10.0f);
        
        renderer->DrawLine(Vector2(scenePos.x, scenePos.y + 22), 
                         Vector2(scenePos.x + sceneSize.x, scenePos.y + 22), 
                         Color(0.0f, 0.0f, 0.0f), 1.0f);
        
        // Unity grid (proper spacing)
        int gridSize = 10;
        int majorGridSize = 100;
        Color gridColor(0.13f, 0.13f, 0.13f);
        Color majorGridColor(0.11f, 0.11f, 0.11f);
        
        // Draw minor grid
        for (float x = scenePos.x; x <= scenePos.x + sceneSize.x; x += gridSize)
        {
            if ((int)(x - scenePos.x) % majorGridSize != 0)
                renderer->DrawLine(Vector2(x, scenePos.y + 22), Vector2(x, scenePos.y + sceneSize.y), gridColor, 0.5f);
        }
        for (float y = scenePos.y + 22; y <= scenePos.y + sceneSize.y; y += gridSize)
        {
            if ((int)(y - scenePos.y - 22) % majorGridSize != 0)
                renderer->DrawLine(Vector2(scenePos.x, y), Vector2(scenePos.x + sceneSize.x, y), gridColor, 0.5f);
        }
        
        // Draw major grid
        for (float x = scenePos.x; x <= scenePos.x + sceneSize.x; x += majorGridSize)
        {
            renderer->DrawLine(Vector2(x, scenePos.y + 22), Vector2(x, scenePos.y + sceneSize.y), majorGridColor, 1.0f);
        }
        for (float y = scenePos.y + 22; y <= scenePos.y + sceneSize.y; y += majorGridSize)
        {
            renderer->DrawLine(Vector2(scenePos.x, y), Vector2(scenePos.x + sceneSize.x, y), majorGridColor, 1.0f);
        }
        
        // Instructions when empty
        if (sceneObjects.empty())
        {
            Vector2 centerPos = scenePos + sceneSize / 2;
            renderer->DrawText(L"Right-click to create objects", 
                             centerPos - Vector2(80, 0), Color(0.4f, 0.4f, 0.4f), L"Segoe UI", 11.0f);
        }
    }

    void RealUnityEditor::DrawInspector(Renderer* renderer)
    {
        // Unity Inspector  
        renderer->FillRectangle(inspectorPos, inspectorSize, Color(0.2f, 0.2f, 0.2f));
        renderer->DrawLine(Vector2(inspectorPos.x, inspectorPos.y), 
                         Vector2(inspectorPos.x, inspectorPos.y + inspectorSize.y), 
                         Color(0.0f, 0.0f, 0.0f), 2.0f);
        
        // Header
        Vector2 headerSize(inspectorSize.x, 22);
        renderer->FillRectangle(inspectorPos, headerSize, Color(0.22f, 0.22f, 0.22f));
        renderer->DrawText(L"Inspector", inspectorPos + Vector2(10, 5), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 11.0f);
        renderer->DrawLine(Vector2(inspectorPos.x, inspectorPos.y + 22), 
                         Vector2(inspectorPos.x + inspectorSize.x, inspectorPos.y + 22), 
                         Color(0.0f, 0.0f, 0.0f), 1.0f);
        
        Vector2 contentPos = inspectorPos + Vector2(10, 30);
        
        if (selectedObject)
        {
            // GameObject icon and name
            renderer->FillRectangle(contentPos, Vector2(16, 16), Color(0.4f, 0.4f, 0.4f));
            renderer->DrawRectangle(contentPos, Vector2(16, 16), Color(0.6f, 0.6f, 0.6f), 1.0f);
            
            std::wstring name = std::wstring(selectedObject->GetName().begin(), selectedObject->GetName().end());
            renderer->DrawText(name, contentPos + Vector2(20, 0), Color(0.95f, 0.95f, 0.95f), L"Segoe UI", 12.0f);
            
            // Active checkbox
            Vector2 checkPos = inspectorPos + Vector2(inspectorSize.x - 30, 30);
            renderer->DrawRectangle(checkPos, Vector2(14, 14), Color(0.5f, 0.5f, 0.5f), 1.0f);
            renderer->FillRectangle(checkPos + Vector2(2, 2), Vector2(10, 10), Color(0.8f, 0.8f, 0.8f));
            
            // Transform component
            Vector2 transformHeader = contentPos + Vector2(-10, 30);
            renderer->FillRectangle(transformHeader, Vector2(inspectorSize.x, 22), Color(0.25f, 0.25f, 0.25f));
            
            // Dropdown arrow
            Vector2 arrowPos = transformHeader + Vector2(5, 8);
            Vector2 arrow[3] = {
                arrowPos,
                arrowPos + Vector2(6, 0),
                arrowPos + Vector2(3, 4)
            };
            renderer->FillTriangle(arrow[0], arrow[1], arrow[2], Color(0.7f, 0.7f, 0.7f));
            
            renderer->DrawText(L"Transform", transformHeader + Vector2(20, 5), Color(0.95f, 0.95f, 0.95f), L"Segoe UI", 10.0f);
            
            auto transform = selectedObject->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetLocalPosition();
                Vector2 propStart = transformHeader + Vector2(15, 28);
                
                renderer->DrawText(L"Position", propStart, Color(0.7f, 0.7f, 0.7f), L"Segoe UI", 9.0f);
                
                // Input fields - these will be functional later
                Vector2 inputPos = propStart + Vector2(50, -2);
                
                // X label and field
                renderer->DrawText(L"X", inputPos - Vector2(10, 0), Color(0.8f, 0.3f, 0.3f), L"Segoe UI", 9.0f);
                renderer->FillRectangle(inputPos, Vector2(50, 18), Color(0.05f, 0.05f, 0.05f));
                renderer->DrawRectangle(inputPos, Vector2(50, 18), Color(0.3f, 0.3f, 0.3f), 1.0f);
                renderer->DrawText(std::to_wstring((int)pos.x), inputPos + Vector2(5, 2), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 9.0f);
                
                // Y label and field
                inputPos.x += 60;
                renderer->DrawText(L"Y", inputPos - Vector2(10, 0), Color(0.3f, 0.8f, 0.3f), L"Segoe UI", 9.0f);
                renderer->FillRectangle(inputPos, Vector2(50, 18), Color(0.05f, 0.05f, 0.05f));
                renderer->DrawRectangle(inputPos, Vector2(50, 18), Color(0.3f, 0.3f, 0.3f), 1.0f);
                renderer->DrawText(std::to_wstring((int)pos.y), inputPos + Vector2(5, 2), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 9.0f);
                
                // Add Component button
                Vector2 addCompBtn = contentPos + Vector2(-10, 100);
                renderer->FillRectangle(addCompBtn, Vector2(inspectorSize.x, 24), Color(0.25f, 0.25f, 0.25f));
                renderer->DrawRectangle(addCompBtn, Vector2(inspectorSize.x, 24), Color(0.4f, 0.4f, 0.4f), 1.0f);
                renderer->DrawText(L"Add Component", addCompBtn + Vector2(inspectorSize.x/2 - 50, 5), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 10.0f);
            }
            
            // SpriteRenderer component
            auto spriteRenderer = selectedObject->GetComponent<SpriteRenderer>();
            if (spriteRenderer)
            {
                Vector2 spriteHeader = contentPos + Vector2(0, 120);
                renderer->FillRectangle(spriteHeader, Vector2(inspectorSize.x - 20, 20), Color(0.22f, 0.22f, 0.22f));
                renderer->DrawText(L"Sprite Renderer", spriteHeader + Vector2(5, 3), Color::White, L"Arial", 11.0f);
                
                // Color field
                Color color = spriteRenderer->GetColor();
                Vector2 colorPos = spriteHeader + Vector2(10, 30);
                renderer->DrawText(L"Color", colorPos, Color(0.8f, 0.8f, 0.8f), L"Arial", 9.0f);
                
                Vector2 colorPreview = colorPos + Vector2(50, 0);
                renderer->FillRectangle(colorPreview, Vector2(60, 15), color);
                renderer->DrawRectangle(colorPreview, Vector2(60, 15), Color::White, 1.0f);
            }
        }
        else
        {
            renderer->DrawText(L"Nothing Selected", contentPos, Color(0.5f, 0.5f, 0.5f), L"Arial", 11.0f);
            renderer->DrawText(L"Select a GameObject to view", contentPos + Vector2(0, 20), Color(0.4f, 0.4f, 0.4f), L"Arial", 9.0f);
            renderer->DrawText(L"its properties in the Inspector", contentPos + Vector2(0, 35), Color(0.4f, 0.4f, 0.4f), L"Arial", 9.0f);
        }
    }

    void RealUnityEditor::DrawProject(Renderer* renderer)
    {
        // Unity Project browser
        renderer->FillRectangle(projectPos, projectSize, Color(0.18f, 0.18f, 0.18f));
        renderer->DrawLine(Vector2(0, projectPos.y), Vector2((float)screenWidth, projectPos.y), Color(0.0f, 0.0f, 0.0f), 2.0f);
        
        // Header
        Vector2 headerSize(projectSize.x, 22);
        renderer->FillRectangle(projectPos, headerSize, Color(0.22f, 0.22f, 0.22f));
        renderer->DrawText(L"Project", projectPos + Vector2(10, 5), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 11.0f);
        renderer->DrawLine(Vector2(0, projectPos.y + 22), 
                         Vector2((float)screenWidth, projectPos.y + 22), 
                         Color(0.0f, 0.0f, 0.0f), 1.0f);
        
        // Folder path bar
        Vector2 pathBarPos = projectPos + Vector2(0, 22);
        Vector2 pathBarSize(projectSize.x, 25);
        renderer->FillRectangle(pathBarPos, pathBarSize, Color(0.15f, 0.15f, 0.15f));
        renderer->DrawText(L"Assets", pathBarPos + Vector2(10, 5), Color(0.8f, 0.8f, 0.8f), L"Segoe UI", 10.0f);
        renderer->DrawLine(Vector2(0, pathBarPos.y + 25), 
                         Vector2((float)screenWidth, pathBarPos.y + 25), 
                         Color(0.1f, 0.1f, 0.1f), 1.0f);
        
        // Asset grid
        Vector2 assetStart = projectPos + Vector2(10, 55);
        int assetsPerRow = (int)(projectSize.x / 90);
        
        for (size_t i = 0; i < assets.size(); i++)
        {
            int row = (int)(i / assetsPerRow);
            int col = (int)(i % assetsPerRow);
            
            Vector2 assetPos = assetStart + Vector2((float)(col * 90), (float)(row * 100));
            Vector2 assetSize(80, 90);
            
            // Asset icon background
            renderer->FillRectangle(assetPos, Vector2(64, 64), Color(0.25f, 0.25f, 0.25f));
            renderer->DrawRectangle(assetPos, Vector2(64, 64), Color(0.35f, 0.35f, 0.35f), 1.0f);
            
            // Asset type icon
            if (assets[i].type == "texture")
            {
                // Image icon
                renderer->FillRectangle(assetPos + Vector2(12, 12), Vector2(40, 40), Color(0.3f, 0.5f, 0.7f));
                renderer->DrawRectangle(assetPos + Vector2(12, 12), Vector2(40, 40), Color(0.4f, 0.6f, 0.8f), 2.0f);
                
                // Mountain icon inside
                Vector2 mountain[3] = {
                    assetPos + Vector2(20, 40),
                    assetPos + Vector2(30, 25),
                    assetPos + Vector2(40, 40)
                };
                renderer->FillTriangle(mountain[0], mountain[1], mountain[2], Color(0.2f, 0.4f, 0.6f));
                
                Vector2 sun = assetPos + Vector2(42, 22);
                renderer->FillCircle(sun, 4, Color(0.9f, 0.8f, 0.3f));
            }
            else if (assets[i].type == "audio")
            {
                // Audio icon
                renderer->FillRectangle(assetPos + Vector2(16, 20), Vector2(32, 24), Color(0.5f, 0.3f, 0.6f));
                renderer->DrawRectangle(assetPos + Vector2(16, 20), Vector2(32, 24), Color(0.6f, 0.4f, 0.7f), 2.0f);
                
                // Wave lines
                for (int w = 0; w < 5; w++)
                {
                    float x = assetPos.x + 20 + w * 6;
                    float height = 8 + (w % 2) * 4;
                    renderer->DrawLine(Vector2(x, assetPos.y + 32 - height/2), 
                                     Vector2(x, assetPos.y + 32 + height/2), 
                                     Color(0.8f, 0.6f, 0.9f), 2.0f);
                }
            }
            
            // Asset name with proper truncation
            std::wstring assetName = std::wstring(assets[i].name.begin(), assets[i].name.end());
            Vector2 textPos = assetPos + Vector2(0, 68);
            Vector2 textSize(64, 20);
            
            // Text background for better readability
            renderer->FillRectangle(textPos, textSize, Color(0.0f, 0.0f, 0.0f, 0.3f));
            
            if (assetName.length() > 10)
                assetName = assetName.substr(0, 8) + L"...";
            
            // Center the text
            float textWidth = assetName.length() * 6; // Approximate
            float textX = textPos.x + (64 - textWidth) / 2;
            renderer->DrawText(assetName, Vector2(textX, textPos.y + 3), Color(0.9f, 0.9f, 0.9f), L"Segoe UI", 9.0f);
        }
        
        // Drop zone text
        if (assets.empty())
        {
            Vector2 centerPos = projectPos + projectSize / 2;
            renderer->DrawText(L"Drag assets here", centerPos - Vector2(50, 0), Color(0.4f, 0.4f, 0.4f), L"Segoe UI", 11.0f);
            
            // Drag drop zone indicator
            Vector2 dropZone = centerPos - Vector2(100, -20);
            Vector2 dropSize(200, 60);
            renderer->DrawRectangle(dropZone, dropSize, Color(0.3f, 0.3f, 0.3f), 2.0f);
            
            // Draw dotted line effect
            float dash = 10.0f;
            for (float x = dropZone.x; x < dropZone.x + dropSize.x; x += dash * 2)
            {
                renderer->DrawLine(Vector2(x, dropZone.y), Vector2(x + dash, dropZone.y), Color(0.4f, 0.4f, 0.4f), 2.0f);
                renderer->DrawLine(Vector2(x, dropZone.y + dropSize.y), Vector2(x + dash, dropZone.y + dropSize.y), Color(0.4f, 0.4f, 0.4f), 2.0f);
            }
            for (float y = dropZone.y; y < dropZone.y + dropSize.y; y += dash * 2)
            {
                renderer->DrawLine(Vector2(dropZone.x, y), Vector2(dropZone.x, y + dash), Color(0.4f, 0.4f, 0.4f), 2.0f);
                renderer->DrawLine(Vector2(dropZone.x + dropSize.x, y), Vector2(dropZone.x + dropSize.x, y + dash), Color(0.4f, 0.4f, 0.4f), 2.0f);
            }
        }
    }

    void RealUnityEditor::DrawCreateMenu(Renderer* renderer)
    {
        Vector2 menuSize(150, 100);
        renderer->FillRectangle(createMenuPos, menuSize, Color(0.2f, 0.2f, 0.2f));
        renderer->DrawRectangle(createMenuPos, menuSize, Color(0.4f, 0.4f, 0.4f), 1.0f);
        
        Vector2 itemPos = createMenuPos + Vector2(10, 10);
        const wchar_t* items[] = {L"Create Empty", L"2D Object > Sprite", L"3D Object > Cube", L"Audio > Audio Source"};
        
        for (int i = 0; i < 4; i++)
        {
            renderer->DrawText(items[i], itemPos, Color::White, L"Arial", 10.0f);
            itemPos.y += 20;
        }
    }

    void RealUnityEditor::HandleInput(Vector2 mousePos, bool leftClick, bool rightClick)
    {
        if (!isEnabled) return;
        
        static bool wasLeftPressed = false;
        static bool wasRightPressed = false;
        
        // Handle dragging
        if (isDragging && leftClick)
        {
            UpdateDrag(mousePos);
        }
        else if (isDragging && !leftClick)
        {
            EndDrag();
        }
        
        // New clicks only
        if (leftClick && !wasLeftPressed)
        {
            // Create button in hierarchy
            Vector2 createBtn = hierarchyPos + Vector2(hierarchySize.x - 50, 5);
            if (IsPointInRect(mousePos, createBtn, Vector2(40, 20)))
            {
                showCreateMenu = !showCreateMenu;
                createMenuPos = createBtn + Vector2(0, 25);
                return;
            }
            
            // Create menu clicks
            if (showCreateMenu && IsPointInRect(mousePos, createMenuPos, Vector2(150, 100)))
            {
                int item = (int)((mousePos.y - (createMenuPos.y + 10)) / 20);
                Vector2 centerPos = scenePos + sceneSize / 2;
                
                switch (item)
                {
                case 0: CreateEmptyGameObject("GameObject", centerPos); break;
                case 1: CreateSpriteGameObject("Sprite", centerPos); break;
                case 2: CreateSpriteGameObject("Cube", centerPos); break;
                }
                showCreateMenu = false;
                return;
            }
            
            // Hierarchy selection
            if (IsPointInRect(mousePos, hierarchyPos, hierarchySize))
            {
                Vector2 listStart = hierarchyPos + Vector2(5, 35);
                int index = (int)((mousePos.y - listStart.y) / 18);
                if (index >= 0 && index < (int)sceneObjects.size())
                {
                    SelectObject(sceneObjects[index].get());
                    selectedIndex = index;
                }
                showCreateMenu = false;
                return;
            }
            
            // Scene view selection and dragging
            if (IsPointInRect(mousePos, scenePos, sceneSize))
            {
                GameObject* clickedObject = nullptr;
                
                for (auto& obj : sceneObjects)
                {
                    auto transform = obj->GetComponent<Transform>();
                    if (transform)
                    {
                        Vector2 objPos = transform->GetPosition();
                        Vector2 objSize(32, 32);
                        
                        auto sprite = obj->GetComponent<SpriteRenderer>();
                        if (sprite)
                            objSize = sprite->GetSize();
                            
                        if (IsPointInRect(mousePos, objPos - objSize/2, objSize))
                        {
                            clickedObject = obj.get();
                            break;
                        }
                    }
                }
                
                if (clickedObject)
                {
                    SelectObject(clickedObject);
                    StartDrag(clickedObject, mousePos);
                }
                else
                {
                    selectedObject = nullptr;
                }
                showCreateMenu = false;
                return;
            }
            
            showCreateMenu = false;
        }
        
        // Right-click in scene
        if (rightClick && !wasRightPressed)
        {
            if (IsPointInRect(mousePos, scenePos, sceneSize))
            {
                GameObject* newObj = CreateEmptyGameObject("GameObject", mousePos);
                SelectObject(newObj);
                DEBUG_LOG("GameObject created at (" + std::to_string((int)mousePos.x) + ", " + std::to_string((int)mousePos.y) + ")");
            }
        }
        
        wasLeftPressed = leftClick;
        wasRightPressed = rightClick;
    }
    
    void RealUnityEditor::HandleKeyInput(int keyCode, bool shift, bool ctrl)
    {
        if (!isEnabled) return;
        
        // Delete key - delete selected object
        if (keyCode == VK_DELETE && selectedObject)
        {
            DeleteSelected();
            DEBUG_LOG("Deleted selected object");
        }
        
        // F2 - rename selected object (later implementation)
        if (keyCode == VK_F2 && selectedObject)
        {
            DEBUG_LOG("Rename not implemented yet");
        }
        
        // Ctrl+D - duplicate selected object
        if (ctrl && keyCode == 'D' && selectedObject)
        {
            auto transform = selectedObject->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetPosition();
                GameObject* newObj = CreateEmptyGameObject(selectedObject->GetName() + "_Copy", pos + Vector2(20, 20));
                
                // Copy components (simplified - only SpriteRenderer for now)
                auto sprite = selectedObject->GetComponent<SpriteRenderer>();
                if (sprite)
                {
                    auto newSprite = newObj->AddComponent<SpriteRenderer>();
                    newSprite->SetColor(sprite->GetColor());
                    newSprite->SetSize(sprite->GetSize());
                }
                
                SelectObject(newObj);
                DEBUG_LOG("Duplicated object: " + newObj->GetName());
            }
        }
        
        // Ctrl+S - save scene (later implementation)
        if (ctrl && keyCode == 'S')
        {
            DEBUG_LOG("Save scene not implemented yet");
        }
        
        // Ctrl+O - open scene (later implementation)
        if (ctrl && keyCode == 'O')
        {
            DEBUG_LOG("Open scene not implemented yet");
        }
        
        // Tool shortcuts
        if (keyCode == 'Q') selectedTool = 0; // Hand
        if (keyCode == 'W') selectedTool = 1; // Move
        if (keyCode == 'E') selectedTool = 2; // Rotate
        if (keyCode == 'R') selectedTool = 3; // Scale
        if (keyCode == 'T') selectedTool = 4; // Rect
    }

    GameObject* RealUnityEditor::CreateEmptyGameObject(const std::string& name, Vector2 position)
    {
        auto obj = std::make_unique<GameObject>(name);
        auto transform = obj->AddComponent<Transform>();
        transform->SetLocalPosition(position);
        
        GameObject* ptr = obj.get();
        sceneObjects.push_back(std::move(obj));
        
        DEBUG_LOG("Created Empty GameObject: " + name + " at (" + std::to_string((int)position.x) + ", " + std::to_string((int)position.y) + ")");
        return ptr;
    }

    GameObject* RealUnityEditor::CreateSpriteGameObject(const std::string& name, Vector2 position)
    {
        GameObject* obj = CreateEmptyGameObject(name, position);
        
        auto sprite = obj->AddComponent<SpriteRenderer>();
        sprite->SetColor(Color::White);
        sprite->SetSize(Vector2(64, 64));
        
        DEBUG_LOG("Created Sprite GameObject: " + name);
        return obj;
    }

    void RealUnityEditor::SelectObject(GameObject* obj)
    {
        selectedObject = obj;
        if (obj)
            DEBUG_LOG("Selected: " + obj->GetName());
    }

    void RealUnityEditor::StartDrag(GameObject* obj, Vector2 mousePos)
    {
        if (obj)
        {
            isDragging = true;
            draggedObject = obj;
            auto transform = obj->GetComponent<Transform>();
            if (transform)
            {
                dragOffset = mousePos - transform->GetPosition();
            }
        }
    }

    void RealUnityEditor::UpdateDrag(Vector2 mousePos)
    {
        if (isDragging && draggedObject)
        {
            auto transform = draggedObject->GetComponent<Transform>();
            if (transform)
            {
                Vector2 newPos = mousePos - dragOffset;
                transform->SetLocalPosition(newPos);
            }
        }
    }

    void RealUnityEditor::EndDrag()
    {
        isDragging = false;
        draggedObject = nullptr;
        dragOffset = Vector2(0, 0);
    }

    void RealUnityEditor::RefreshAssets()
    {
        assets.clear();
        
        // Scan Assets folder for supported files
        if (std::filesystem::exists(projectPath))
        {
            for (const auto& entry : std::filesystem::directory_iterator(projectPath))
            {
                if (entry.is_regular_file())
                {
                    std::string path = entry.path().string();
                    std::string name = entry.path().filename().string();
                    std::string ext = entry.path().extension().string();
                    
                    AssetItem asset;
                    asset.name = name;
                    asset.path = path;
                    
                    // Determine type
                    if (ext == ".png" || ext == ".jpg" || ext == ".bmp")
                    {
                        asset.type = "texture";
                        // Load texture
                        asset.texture = TextureManager::GetInstance().LoadTexture(path);
                    }
                    else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg")
                    {
                        asset.type = "audio";
                        // Load audio
                        asset.audio = AudioManager::GetInstance().LoadAudioClip(path);
                    }
                    
                    assets.push_back(asset);
                    DEBUG_LOG("Asset loaded: " + name + " (" + asset.type + ")");
                }
            }
        }
    }

    void RealUnityEditor::LoadImageFile(const std::string& filePath)
    {
        auto texture = TextureManager::GetInstance().LoadTexture(filePath);
        if (texture)
        {
            DEBUG_LOG("Image loaded successfully: " + filePath);
            RefreshAssets();
        }
    }

    void RealUnityEditor::LoadAudioFile(const std::string& filePath)
    {
        auto audio = AudioManager::GetInstance().LoadAudioClip(filePath);
        if (audio)
        {
            DEBUG_LOG("Audio loaded successfully: " + filePath);
            RefreshAssets();
        }
    }

    void RealUnityEditor::DeleteSelected()
    {
        if (selectedObject)
        {
            sceneObjects.erase(
                std::remove_if(sceneObjects.begin(), sceneObjects.end(),
                    [this](const std::unique_ptr<GameObject>& obj) {
                        return obj.get() == selectedObject;
                    }),
                sceneObjects.end()
            );
            selectedObject = nullptr;
        }
    }

    bool RealUnityEditor::IsPointInRect(Vector2 point, Vector2 pos, Vector2 size)
    {
        return point.x >= pos.x && point.x <= pos.x + size.x &&
               point.y >= pos.y && point.y <= pos.y + size.y;
    }
    
    void RealUnityEditor::HandleDragDrop(const std::wstring& filePath)
    {
        // Convert wide string to narrow string
        std::string path(filePath.begin(), filePath.end());
        
        // Get file extension
        size_t dotPos = path.find_last_of('.');
        if (dotPos == std::string::npos) return;
        
        std::string ext = path.substr(dotPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Check file type and load
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
        {
            // Copy to Assets folder
            std::string fileName = path.substr(path.find_last_of("/\\") + 1);
            std::string destPath = projectPath + "/" + fileName;
            
            // Use Windows file copy
            std::wstring wSrc = filePath;
            std::wstring wDest(destPath.begin(), destPath.end());
            CopyFileW(wSrc.c_str(), wDest.c_str(), FALSE);
            
            DEBUG_LOG("Image file dropped: " + fileName);
            RefreshAssets();
        }
        else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg")
        {
            // Copy to Assets folder
            std::string fileName = path.substr(path.find_last_of("/\\") + 1);
            std::string destPath = projectPath + "/" + fileName;
            
            // Use Windows file copy
            std::wstring wSrc = filePath;
            std::wstring wDest(destPath.begin(), destPath.end());
            CopyFileW(wSrc.c_str(), wDest.c_str(), FALSE);
            
            DEBUG_LOG("Audio file dropped: " + fileName);
            RefreshAssets();
        }
        else
        {
            DEBUG_LOG("Unsupported file type: " + ext);
        }
    }
}