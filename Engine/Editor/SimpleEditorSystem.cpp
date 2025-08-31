#include "framework.h"
#include "SimpleEditorSystem.h"
#include "../Core/Renderer.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include "../Components/SpriteRenderer.h"
#include "../Utils/Color.h"
#include <functional>

namespace AronEngine
{
    SimpleEditorSystem::SimpleEditorSystem()
        : isEnabled(true)
        , showHierarchy(true)
        , showInspector(true)
        , selectedObject(nullptr)
        , mousePos(0, 0)
        , mousePressed(false)
        , rightClickPressed(false)
        , showCreateMenu(false)
        , createMenuPos(0, 0)
    {
    }

    SimpleEditorSystem::~SimpleEditorSystem()
    {
        Shutdown();
    }

    bool SimpleEditorSystem::Initialize()
    {
        SetupButtons();
        DEBUG_LOG("Simple Editor System initialized");
        return true;
    }

    void SimpleEditorSystem::Shutdown()
    {
        gameObjects.clear();
        selectedObject = nullptr;
        buttons.clear();
        DEBUG_LOG("Simple Editor System shutdown");
    }

    void SimpleEditorSystem::Update(float deltaTime)
    {
        if (!isEnabled) return;
    }

    void SimpleEditorSystem::Render(Renderer* renderer)
    {
        if (!isEnabled || !renderer) return;

        // Unity-style layout
        int screenWidth = 800; // Assume screen width
        int screenHeight = 600;
        
        // Hierarchy panel (left side, like Unity)
        if (showHierarchy)
            RenderHierarchy(renderer);
            
        // Inspector panel (right side, like Unity)  
        if (showInspector)
            RenderInspector(renderer);
            
        // Top toolbar (like Unity)
        RenderToolbar(renderer);
        
        // Scene view instructions
        renderer->DrawText(L"Scene View: Right-click to create objects", 
                         Vector2(320, screenHeight - 40), Color::Yellow, L"Arial", 14.0f);
        renderer->DrawText(L"Click objects to select them", 
                         Vector2(320, screenHeight - 20), Color::Cyan, L"Arial", 12.0f);
    }

    void SimpleEditorSystem::RenderHierarchy(Renderer* renderer)
    {
        // Unity-style Hierarchy panel (left side)
        Vector2 hierarchyPos(10, 40);
        Vector2 hierarchySize(250, 400);
        
        renderer->FillRectangle(hierarchyPos, hierarchySize, Color(0.12f, 0.12f, 0.12f, 0.95f));
        renderer->DrawRectangle(hierarchyPos, hierarchySize, Color(0.3f, 0.3f, 0.3f), 1.0f);
        
        // Title bar
        renderer->FillRectangle(hierarchyPos, Vector2(hierarchySize.x, 25), Color(0.2f, 0.2f, 0.2f));
        renderer->DrawText(L"Hierarchy", hierarchyPos + Vector2(10, 5), Color::White, L"Arial", 14.0f);
        
        // Create dropdown button (Unity-style)
        Vector2 createBtnPos = hierarchyPos + Vector2(170, 3);
        Vector2 createBtnSize(70, 20);
        renderer->FillRectangle(createBtnPos, createBtnSize, Color(0.3f, 0.5f, 0.3f));
        renderer->DrawText(L"Create +", createBtnPos + Vector2(5, 3), Color::White, L"Arial", 11.0f);
        
        // GameObject list
        Vector2 listStart = hierarchyPos + Vector2(5, 35);
        
        if (gameObjects.empty())
        {
            renderer->DrawText(L"(Empty Scene)", listStart + Vector2(5, 10), Color::Gray, L"Arial", 12.0f);
            renderer->DrawText(L"Right-click in Scene View", listStart + Vector2(5, 30), Color::Gray, L"Arial", 10.0f);
            renderer->DrawText(L"or use Create button", listStart + Vector2(5, 45), Color::Gray, L"Arial", 10.0f);
        }
        else
        {
            for (size_t i = 0; i < gameObjects.size(); ++i)
            {
                Vector2 itemPos = listStart + Vector2(5, (float)(i * 25));
                Vector2 itemSize(235, 22);
                
                bool isSelected = (selectedObject == gameObjects[i].get());
                Color bgColor = isSelected ? Color(0.25f, 0.4f, 0.6f) : Color(0.18f, 0.18f, 0.18f);
                
                // Item background
                renderer->FillRectangle(itemPos, itemSize, bgColor);
                if (isSelected)
                {
                    renderer->DrawRectangle(itemPos, itemSize, Color::Cyan, 1.0f);
                }
                
                // GameObject icon (Unity-style)
                renderer->FillRectangle(itemPos + Vector2(3, 3), Vector2(16, 16), Color(0.4f, 0.4f, 0.8f));
                renderer->DrawText(L"O", itemPos + Vector2(5, 3), Color::White, L"Arial", 12.0f);
                
                // GameObject name
                std::wstring objName = std::wstring(gameObjects[i]->GetName().begin(), gameObjects[i]->GetName().end());
                Color textColor = isSelected ? Color::White : Color(0.9f, 0.9f, 0.9f);
                renderer->DrawText(objName, itemPos + Vector2(25, 5), textColor, L"Arial", 12.0f);
                
                // Active checkbox
                Vector2 checkPos = itemPos + Vector2(itemSize.x - 20, 5);
                bool isActive = gameObjects[i]->IsActive();
                renderer->DrawRectangle(checkPos, Vector2(12, 12), Color::Gray, 1.0f);
                if (isActive)
                {
                    renderer->FillRectangle(checkPos + Vector2(2, 2), Vector2(8, 8), Color::Green);
                }
            }
        }
    }

    void SimpleEditorSystem::RenderInspector(Renderer* renderer)
    {
        // Unity-style Inspector panel (right side)
        int screenWidth = 800;
        Vector2 inspectorPos(screenWidth - 260, 40);
        Vector2 inspectorSize(250, 400);
        
        renderer->FillRectangle(inspectorPos, inspectorSize, Color(0.12f, 0.12f, 0.12f, 0.95f));
        renderer->DrawRectangle(inspectorPos, inspectorSize, Color(0.3f, 0.3f, 0.3f), 1.0f);
        
        // Title bar
        renderer->FillRectangle(inspectorPos, Vector2(inspectorSize.x, 25), Color(0.2f, 0.2f, 0.2f));
        renderer->DrawText(L"Inspector", inspectorPos + Vector2(10, 5), Color::White, L"Arial", 14.0f);
        
        Vector2 contentPos = inspectorPos + Vector2(10, 35);
        
        if (selectedObject)
        {
            // GameObject name (editable in Unity)
            std::wstring objName = std::wstring(selectedObject->GetName().begin(), selectedObject->GetName().end());
            renderer->DrawText(objName, contentPos, Color::White, L"Arial", 14.0f);
            
            // Active checkbox
            Vector2 activePos = contentPos + Vector2(0, 25);
            renderer->DrawRectangle(activePos, Vector2(15, 15), Color::Gray, 1.0f);
            if (selectedObject->IsActive())
            {
                renderer->FillRectangle(activePos + Vector2(2, 2), Vector2(11, 11), Color::Green);
            }
            renderer->DrawText(L"Active", activePos + Vector2(20, 0), Color::White, L"Arial", 12.0f);
            
            // Transform component (Unity-style)
            Vector2 transformPos = contentPos + Vector2(0, 55);
            renderer->DrawText(L"v Transform", transformPos, Color::Cyan, L"Arial", 13.0f);
            
            auto transform = selectedObject->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetLocalPosition();
                Vector2 scale = transform->GetLocalScale();
                float rotation = transform->GetLocalRotation();
                
                Vector2 propPos = transformPos + Vector2(10, 25);
                renderer->DrawText(L"Position", propPos, Color::White, L"Arial", 11.0f);
                std::wstring posText = L"X:" + std::to_wstring((int)pos.x) + L" Y:" + std::to_wstring((int)pos.y);
                renderer->DrawText(posText, propPos + Vector2(0, 15), Color::Cyan, L"Arial", 10.0f);
                
                propPos.y += 40;
                renderer->DrawText(L"Rotation", propPos, Color::White, L"Arial", 11.0f);
                std::wstring rotText = L"Z:" + std::to_wstring((int)rotation);
                renderer->DrawText(rotText, propPos + Vector2(0, 15), Color::Cyan, L"Arial", 10.0f);
                
                propPos.y += 40;
                renderer->DrawText(L"Scale", propPos, Color::White, L"Arial", 11.0f);
                std::wstring scaleText = L"X:" + std::to_wstring(scale.x).substr(0, 4) + L" Y:" + std::to_wstring(scale.y).substr(0, 4);
                renderer->DrawText(scaleText, propPos + Vector2(0, 15), Color::Cyan, L"Arial", 10.0f);
            }
            
            // SpriteRenderer component (if exists)
            auto spriteRenderer = selectedObject->GetComponent<SpriteRenderer>();
            if (spriteRenderer)
            {
                Vector2 spritePos = contentPos + Vector2(0, 195);
                renderer->DrawText(L"v Sprite Renderer", spritePos, Color::Green, L"Arial", 13.0f);
                
                Color color = spriteRenderer->GetColor();
                Vector2 colorPos = spritePos + Vector2(10, 25);
                renderer->DrawText(L"Color", colorPos, Color::White, L"Arial", 11.0f);
                
                // Color preview
                Vector2 colorPreview = colorPos + Vector2(50, 0);
                renderer->FillRectangle(colorPreview, Vector2(40, 15), color);
                renderer->DrawRectangle(colorPreview, Vector2(40, 15), Color::White, 1.0f);
                
                Vector2 size = spriteRenderer->GetSize();
                Vector2 sizePos = colorPos + Vector2(0, 25);
                renderer->DrawText(L"Size", sizePos, Color::White, L"Arial", 11.0f);
                std::wstring sizeText = L"W:" + std::to_wstring((int)size.x) + L" H:" + std::to_wstring((int)size.y);
                renderer->DrawText(sizeText, sizePos + Vector2(0, 15), Color::Cyan, L"Arial", 10.0f);
            }
        }
        else
        {
            renderer->DrawText(L"Nothing Selected", contentPos, Color::Gray, L"Arial", 12.0f);
            renderer->DrawText(L"Select a GameObject", contentPos + Vector2(0, 20), Color::Gray, L"Arial", 11.0f);
            renderer->DrawText(L"to view its properties", contentPos + Vector2(0, 35), Color::Gray, L"Arial", 11.0f);
        }
    }

    void SimpleEditorSystem::RenderToolbar(Renderer* renderer)
    {
        // Unity-style top toolbar
        Vector2 toolbarPos(10, 10);
        Vector2 toolbarSize(780, 25);
        
        renderer->FillRectangle(toolbarPos, toolbarSize, Color(0.15f, 0.15f, 0.15f, 0.95f));
        renderer->DrawRectangle(toolbarPos, toolbarSize, Color(0.4f, 0.4f, 0.4f), 1.0f);
        
        // Tool buttons
        Vector2 btnPos = toolbarPos + Vector2(10, 3);
        Vector2 btnSize(60, 18);
        
        // Transform tools (Unity-style)
        renderer->FillRectangle(btnPos, btnSize, Color(0.3f, 0.3f, 0.3f));
        renderer->DrawText(L"Move", btnPos + Vector2(15, 2), Color::White, L"Arial", 10.0f);
        
        btnPos.x += 65;
        renderer->FillRectangle(btnPos, btnSize, Color(0.3f, 0.3f, 0.3f));
        renderer->DrawText(L"Rotate", btnPos + Vector2(10, 2), Color::White, L"Arial", 10.0f);
        
        btnPos.x += 65;
        renderer->FillRectangle(btnPos, btnSize, Color(0.3f, 0.3f, 0.3f));
        renderer->DrawText(L"Scale", btnPos + Vector2(12, 2), Color::White, L"Arial", 10.0f);
        
        // Play button (Unity-style)
        btnPos.x = toolbarPos.x + 350;
        renderer->FillRectangle(btnPos, btnSize, Color(0.2f, 0.5f, 0.2f));
        renderer->DrawText(L"> Play", btnPos + Vector2(8, 2), Color::White, L"Arial", 10.0f);
    }

    void SimpleEditorSystem::RenderButtons(Renderer* renderer)
    {
        // Scene view area outline
        Vector2 scenePos(270, 40);
        Vector2 sceneSize(270, 400);
        renderer->DrawRectangle(scenePos, sceneSize, Color(0.4f, 0.4f, 0.4f), 2.0f);
        renderer->DrawText(L"Scene", scenePos + Vector2(10, -25), Color::White, L"Arial", 14.0f);
        
        // Create menu (Unity-style dropdown)
        if (showCreateMenu)
        {
            Vector2 menuSize(120, 100);
            renderer->FillRectangle(createMenuPos, menuSize, Color(0.2f, 0.2f, 0.2f, 0.95f));
            renderer->DrawRectangle(createMenuPos, menuSize, Color(0.5f, 0.5f, 0.5f), 1.0f);
            
            // Menu items
            Vector2 itemPos = createMenuPos + Vector2(5, 5);
            Vector2 itemSize(110, 18);
            
            // Empty GameObject
            renderer->FillRectangle(itemPos, itemSize, Color(0.25f, 0.25f, 0.25f));
            renderer->DrawText(L"Empty GameObject", itemPos + Vector2(5, 2), Color::White, L"Arial", 10.0f);
            
            itemPos.y += 20;
            renderer->FillRectangle(itemPos, itemSize, Color(0.25f, 0.25f, 0.25f));
            renderer->DrawText(L"Sprite", itemPos + Vector2(5, 2), Color::White, L"Arial", 10.0f);
            
            itemPos.y += 20;
            renderer->FillRectangle(itemPos, itemSize, Color(0.25f, 0.25f, 0.25f));
            renderer->DrawText(L"Cube", itemPos + Vector2(5, 2), Color::White, L"Arial", 10.0f);
            
            itemPos.y += 20;
            renderer->FillRectangle(itemPos, itemSize, Color(0.25f, 0.25f, 0.25f));
            renderer->DrawText(L"Camera", itemPos + Vector2(5, 2), Color::Gray, L"Arial", 10.0f);
        }
    }

    void SimpleEditorSystem::HandleMouseInput(Vector2 mousePosition, bool leftPressed, bool rightPressed)
    {
        if (!isEnabled) return;
        
        mousePos = mousePosition;
        
        if (leftPressed && !mousePressed)
        {
            // 1. Check hierarchy panel clicks (left side)
            Vector2 hierarchyPos(10, 40);
            Vector2 hierarchySize(250, 400);
            
            if (IsPointInRect(mousePos, hierarchyPos, hierarchySize))
            {
                // Create button click
                Vector2 createBtnPos = hierarchyPos + Vector2(170, 3);
                Vector2 createBtnSize(70, 20);
                if (IsPointInRect(mousePos, createBtnPos, createBtnSize))
                {
                    showCreateMenu = !showCreateMenu;
                    createMenuPos = createBtnPos + Vector2(0, 25);
                    DEBUG_LOG("Create menu toggled");
                    return;
                }
                
                // Handle create menu clicks
                if (showCreateMenu)
                {
                    Vector2 menuSize(120, 100);
                    if (IsPointInRect(mousePos, createMenuPos, menuSize))
                    {
                        Vector2 itemPos = createMenuPos + Vector2(5, 5);
                        int menuIndex = (int)((mousePos.y - itemPos.y) / 20);
                        
                        switch (menuIndex)
                        {
                        case 0: // Empty GameObject
                            CreateEmptyGameObject("GameObject");
                            break;
                        case 1: // Sprite
                            CreateSpriteObject("Sprite");
                            break;
                        case 2: // Cube
                            CreateCubeObject("Cube");
                            break;
                        default:
                            break;
                        }
                        showCreateMenu = false;
                        return;
                    }
                }
                
                // GameObject selection in list
                Vector2 listStart = hierarchyPos + Vector2(10, 40);
                int clickedIndex = (int)((mousePos.y - listStart.y) / 25);
                
                if (clickedIndex >= 0 && clickedIndex < (int)gameObjects.size())
                {
                    selectedObject = gameObjects[clickedIndex].get();
                    DEBUG_LOG("Object selected in hierarchy: " + selectedObject->GetName());
                }
                return;
            }
            else
            {
                // Close create menu if clicking outside
                showCreateMenu = false;
            }
            
            // 2. Check scene view clicks (middle area) - Unity-style object selection
            Vector2 sceneStart(270, 40);
            Vector2 sceneEnd(540, 440);
            
            if (mousePos.x >= sceneStart.x && mousePos.x <= sceneEnd.x && 
                mousePos.y >= sceneStart.y && mousePos.y <= sceneEnd.y)
            {
                // Try to select object by clicking on it in scene view
                selectedObject = nullptr;
                
                for (auto& gameObject : gameObjects)
                {
                    auto transform = gameObject->GetComponent<Transform>();
                    if (transform)
                    {
                        Vector2 objPos = transform->GetLocalPosition();
                        Vector2 objSize(64, 64); // Default size
                        
                        auto spriteRenderer = gameObject->GetComponent<SpriteRenderer>();
                        if (spriteRenderer)
                        {
                            objSize = spriteRenderer->GetSize();
                        }
                        
                        // Check if mouse clicked on this object
                        if (IsPointInRect(mousePos, objPos, objSize))
                        {
                            selectedObject = gameObject.get();
                            DEBUG_LOG("Object selected in scene view: " + selectedObject->GetName());
                            break;
                        }
                    }
                }
            }
        }
        
        // Right-click context menu in scene view (Unity-style)
        if (rightPressed && !rightClickPressed)
        {
            Vector2 sceneStart(270, 40);
            Vector2 sceneEnd(540, 440);
            
            if (mousePos.x >= sceneStart.x && mousePos.x <= sceneEnd.x && 
                mousePos.y >= sceneStart.y && mousePos.y <= sceneEnd.y)
            {
                // Unity-style: Right-click creates empty GameObject at mouse position
                GameObject* newObj = CreateEmptyGameObject("GameObject");
                auto transform = newObj->GetComponent<Transform>();
                if (transform)
                {
                    transform->SetLocalPosition(mousePos);
                }
                selectedObject = newObj;
                DEBUG_LOG("Empty GameObject created at mouse position: (" + 
                         std::to_string((int)mousePos.x) + ", " + std::to_string((int)mousePos.y) + ")");
            }
        }
        
        mousePressed = leftPressed;
        rightClickPressed = rightPressed;
    }

    GameObject* SimpleEditorSystem::CreateGameObject(const std::string& name)
    {
        auto gameObject = std::make_unique<GameObject>(name);
        gameObject->AddComponent<Transform>();
        
        GameObject* ptr = gameObject.get();
        gameObjects.push_back(std::move(gameObject));
        
        DEBUG_LOG("GameObject created: " + name);
        return ptr;
    }

    GameObject* SimpleEditorSystem::CreateSpriteObject(const std::string& name)
    {
        GameObject* obj = CreateGameObject(name);
        
        auto spriteRenderer = obj->AddComponent<SpriteRenderer>();
        spriteRenderer->SetColor(Color::White); // Unity default white
        spriteRenderer->SetSize(Vector2(64, 64));
        
        // Set position at center of scene view or mouse position
        auto transform = obj->GetComponent<Transform>();
        Vector2 defaultPos(400, 300); // Center of screen
        transform->SetLocalPosition(defaultPos);
        
        DEBUG_LOG("Sprite GameObject created: " + name);
        return obj;
    }
    
    GameObject* SimpleEditorSystem::CreateEmptyGameObject(const std::string& name)
    {
        GameObject* obj = CreateGameObject(name);
        
        // Only Transform component (Unity default)
        auto transform = obj->GetComponent<Transform>();
        Vector2 defaultPos(400, 300);
        transform->SetLocalPosition(defaultPos);
        
        DEBUG_LOG("Empty GameObject created: " + name);
        return obj;
    }

    GameObject* SimpleEditorSystem::CreateCubeObject(const std::string& name)
    {
        GameObject* obj = CreateGameObject(name);
        
        auto spriteRenderer = obj->AddComponent<SpriteRenderer>();
        spriteRenderer->SetColor(Color(0.8f, 0.8f, 0.8f)); // Unity default gray
        spriteRenderer->SetSize(Vector2(64, 64));
        
        auto transform = obj->GetComponent<Transform>();
        Vector2 defaultPos(400, 300);
        transform->SetLocalPosition(defaultPos);
        
        DEBUG_LOG("Cube GameObject created: " + name);
        return obj;
    }

    void SimpleEditorSystem::DeleteSelectedObject()
    {
        if (selectedObject)
        {
            gameObjects.erase(
                std::remove_if(gameObjects.begin(), gameObjects.end(),
                    [this](const std::unique_ptr<GameObject>& obj) {
                        return obj.get() == selectedObject;
                    }),
                gameObjects.end()
            );
            selectedObject = nullptr;
            DEBUG_LOG("Selected object deleted");
        }
    }

    bool SimpleEditorSystem::IsPointInRect(Vector2 point, Vector2 rectPos, Vector2 rectSize)
    {
        return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x &&
               point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
    }

    void SimpleEditorSystem::SetupButtons()
    {
        buttons.clear();
        
        // Create Sprite button
        EditorButton createSpriteBtn(Vector2(330, 50), Vector2(100, 30), L"Create Sprite");
        createSpriteBtn.onClick = [this]() { CreateSpriteObject("Sprite"); };
        buttons.push_back(createSpriteBtn);
        
        // Delete button
        EditorButton deleteBtn(Vector2(330, 90), Vector2(100, 30), L"Delete");
        deleteBtn.onClick = [this]() { DeleteSelectedObject(); };
        buttons.push_back(deleteBtn);
        
        // Clear All button
        EditorButton clearBtn(Vector2(330, 130), Vector2(100, 30), L"Clear All");
        clearBtn.onClick = [this]() { 
            gameObjects.clear(); 
            selectedObject = nullptr; 
            DEBUG_LOG("All objects cleared"); 
        };
        buttons.push_back(clearBtn);
    }
}