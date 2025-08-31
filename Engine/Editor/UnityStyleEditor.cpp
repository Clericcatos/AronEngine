#include "framework.h"
#include "UnityStyleEditor.h"
#include "../Core/Renderer.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include "../Components/SpriteRenderer.h"

namespace AronEngine
{
    UnityStyleEditor::UnityStyleEditor()
        : isEnabled(true)
        , selectedObject(nullptr)
        , showCreateDropdown(false)
        , createDropdownPos(0, 0)
    {
    }

    UnityStyleEditor::~UnityStyleEditor()
    {
        Shutdown();
    }

    bool UnityStyleEditor::Initialize()
    {
        DEBUG_LOG("Unity Style Editor initialized - Clean empty scene");
        return true;
    }

    void UnityStyleEditor::Shutdown()
    {
        sceneObjects.clear();
        selectedObject = nullptr;
        DEBUG_LOG("Unity Style Editor shutdown");
    }

    void UnityStyleEditor::Update(float deltaTime)
    {
    }

    void UnityStyleEditor::Render(Renderer* renderer)
    {
        if (!isEnabled || !renderer) return;

        DrawToolbar(renderer);
        DrawHierarchy(renderer);
        DrawInspector(renderer);
        DrawSceneView(renderer);
        
        if (showCreateDropdown)
            DrawCreateDropdown(renderer);
    }

    void UnityStyleEditor::DrawToolbar(Renderer* renderer)
    {
        // Clean Unity toolbar
        Vector2 pos(0, 0);
        Vector2 size(800, 30);
        
        renderer->FillRectangle(pos, size, Color(0.22f, 0.22f, 0.22f));
        renderer->DrawLine(Vector2(0, 30), Vector2(800, 30), Color(0.1f, 0.1f, 0.1f), 1.0f);
        
        // Tool buttons
        Vector2 btnPos(10, 6);
        Vector2 btnSize(50, 18);
        
        const wchar_t* tools[] = {L"Move", L"Rotate", L"Scale"};
        for (int i = 0; i < 3; i++)
        {
            renderer->FillRectangle(btnPos, btnSize, Color(0.3f, 0.3f, 0.3f));
            renderer->DrawText(tools[i], btnPos + Vector2(8, 2), Color::White, L"Arial", 9.0f);
            btnPos.x += 55;
        }
        
        // Play button
        btnPos.x = 350;
        renderer->FillRectangle(btnPos, btnSize, Color(0.2f, 0.4f, 0.2f));
        renderer->DrawText(L"Play", btnPos + Vector2(12, 2), Color::White, L"Arial", 9.0f);
    }

    void UnityStyleEditor::DrawHierarchy(Renderer* renderer)
    {
        // Unity Hierarchy panel
        Vector2 pos(0, 30);
        Vector2 size(250, 400);
        
        renderer->FillRectangle(pos, size, Color(0.18f, 0.18f, 0.18f));
        renderer->DrawLine(Vector2(250, 30), Vector2(250, 430), Color(0.1f, 0.1f, 0.1f), 1.0f);
        
        // Header
        Vector2 headerPos = pos + Vector2(0, 0);
        Vector2 headerSize(size.x, 25);
        renderer->FillRectangle(headerPos, headerSize, Color(0.15f, 0.15f, 0.15f));
        renderer->DrawText(L"Hierarchy", headerPos + Vector2(10, 5), Color::White, L"Arial", 12.0f);
        
        // Create button
        Vector2 createBtn = headerPos + Vector2(180, 3);
        Vector2 createBtnSize(60, 18);
        renderer->FillRectangle(createBtn, createBtnSize, Color(0.3f, 0.3f, 0.3f));
        renderer->DrawText(L"Create", createBtn + Vector2(10, 2), Color::White, L"Arial", 9.0f);
        
        // Scene objects list
        Vector2 listPos = pos + Vector2(5, 30);
        
        if (sceneObjects.empty())
        {
            renderer->DrawText(L"(empty)", listPos + Vector2(10, 15), Color(0.5f, 0.5f, 0.5f), L"Arial", 11.0f);
        }
        else
        {
            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                Vector2 itemPos = listPos + Vector2(0, (float)(i * 22));
                Vector2 itemSize(240, 20);
                
                bool isSelected = (selectedObject == sceneObjects[i].get());
                
                if (isSelected)
                {
                    renderer->FillRectangle(itemPos, itemSize, Color(0.25f, 0.4f, 0.6f));
                }
                
                // Object name
                std::wstring name = std::wstring(sceneObjects[i]->GetName().begin(), sceneObjects[i]->GetName().end());
                Color textColor = isSelected ? Color::White : Color(0.85f, 0.85f, 0.85f);
                renderer->DrawText(name, itemPos + Vector2(20, 2), textColor, L"Arial", 11.0f);
                
                // Icon
                renderer->FillRectangle(itemPos + Vector2(2, 2), Vector2(16, 16), Color(0.4f, 0.4f, 0.6f));
            }
        }
    }

    void UnityStyleEditor::DrawInspector(Renderer* renderer)
    {
        // Unity Inspector panel  
        Vector2 pos(550, 30);
        Vector2 size(250, 400);
        
        renderer->FillRectangle(pos, size, Color(0.18f, 0.18f, 0.18f));
        renderer->DrawLine(Vector2(550, 30), Vector2(550, 430), Color(0.1f, 0.1f, 0.1f), 1.0f);
        
        // Header
        Vector2 headerPos = pos;
        Vector2 headerSize(size.x, 25);
        renderer->FillRectangle(headerPos, headerSize, Color(0.15f, 0.15f, 0.15f));
        renderer->DrawText(L"Inspector", headerPos + Vector2(10, 5), Color::White, L"Arial", 12.0f);
        
        Vector2 contentPos = pos + Vector2(10, 35);
        
        if (selectedObject)
        {
            std::wstring name = std::wstring(selectedObject->GetName().begin(), selectedObject->GetName().end());
            renderer->DrawText(name, contentPos, Color::White, L"Arial", 13.0f);
            
            // Transform component
            Vector2 transformPos = contentPos + Vector2(0, 30);
            renderer->DrawText(L"Transform", transformPos, Color(0.7f, 0.9f, 1.0f), L"Arial", 12.0f);
            
            auto transform = selectedObject->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetLocalPosition();
                Vector2 propPos = transformPos + Vector2(10, 20);
                
                renderer->DrawText(L"Position", propPos, Color(0.8f, 0.8f, 0.8f), L"Arial", 10.0f);
                std::wstring posStr = L"X: " + std::to_wstring((int)pos.x) + L"  Y: " + std::to_wstring((int)pos.y);
                renderer->DrawText(posStr, propPos + Vector2(0, 15), Color::White, L"Arial", 9.0f);
            }
            
            // SpriteRenderer (if exists)
            auto spriteRenderer = selectedObject->GetComponent<SpriteRenderer>();
            if (spriteRenderer)
            {
                Vector2 spritePos = contentPos + Vector2(0, 120);
                renderer->DrawText(L"Sprite Renderer", spritePos, Color(0.7f, 1.0f, 0.7f), L"Arial", 12.0f);
            }
        }
        else
        {
            renderer->DrawText(L"Nothing Selected", contentPos, Color(0.5f, 0.5f, 0.5f), L"Arial", 12.0f);
        }
    }

    void UnityStyleEditor::DrawSceneView(Renderer* renderer)
    {
        // Clean scene view area
        Vector2 pos(250, 30);
        Vector2 size(300, 400);
        
        // Scene view border
        renderer->DrawRectangle(pos, size, Color(0.3f, 0.3f, 0.3f), 1.0f);
        
        // Scene tab
        Vector2 tabPos(pos.x + 5, pos.y - 20);
        renderer->DrawText(L"Scene", tabPos, Color::White, L"Arial", 11.0f);
        
        // Grid (subtle)
        for (int x = (int)pos.x; x < pos.x + size.x; x += 50)
        {
            renderer->DrawLine(Vector2((float)x, pos.y), Vector2((float)x, pos.y + size.y), Color(0.15f, 0.15f, 0.15f), 1.0f);
        }
        for (int y = (int)pos.y; y < pos.y + size.y; y += 50)
        {
            renderer->DrawLine(Vector2(pos.x, (float)y), Vector2(pos.x + size.x, (float)y), Color(0.15f, 0.15f, 0.15f), 1.0f);
        }
    }

    void UnityStyleEditor::DrawCreateDropdown(Renderer* renderer)
    {
        Vector2 menuSize(140, 80);
        renderer->FillRectangle(createDropdownPos, menuSize, Color(0.2f, 0.2f, 0.2f));
        renderer->DrawRectangle(createDropdownPos, menuSize, Color(0.4f, 0.4f, 0.4f), 1.0f);
        
        Vector2 itemPos = createDropdownPos + Vector2(5, 5);
        const wchar_t* items[] = {L"Empty GameObject", L"Sprite", L"Cube"};
        
        for (int i = 0; i < 3; i++)
        {
            Vector2 itemSize(130, 20);
            renderer->DrawText(items[i], itemPos + Vector2(5, 2), Color::White, L"Arial", 10.0f);
            itemPos.y += 22;
        }
    }

    void UnityStyleEditor::HandleInput(Vector2 mousePos, bool leftClick, bool rightClick)
    {
        if (!isEnabled) return;
        
        static bool wasLeftPressed = false;
        static bool wasRightPressed = false;
        
        // Only process on new clicks
        if (leftClick && !wasLeftPressed)
        {
            // Hierarchy create button
            Vector2 createBtn(180, 33);
            Vector2 createBtnSize(60, 18);
            if (IsPointInRect(mousePos, createBtn, createBtnSize))
            {
                showCreateDropdown = !showCreateDropdown;
                createDropdownPos = createBtn + Vector2(0, 20);
                return;
            }
            
            // Create dropdown clicks
            if (showCreateDropdown && IsPointInRect(mousePos, createDropdownPos, Vector2(140, 80)))
            {
                int item = (int)((mousePos.y - (createDropdownPos.y + 5)) / 22);
                Vector2 centerPos(400, 250);
                
                switch (item)
                {
                case 0: CreateEmpty("GameObject", centerPos); break;
                case 1: CreateSprite("Sprite", centerPos); break;  
                case 2: CreateCube("Cube", centerPos); break;
                }
                showCreateDropdown = false;
                return;
            }
            
            // Hierarchy object selection
            if (mousePos.x < 250 && mousePos.y > 55)
            {
                int index = (int)((mousePos.y - 60) / 22);
                if (index >= 0 && index < (int)sceneObjects.size())
                {
                    SelectObject(sceneObjects[index].get());
                }
                return;
            }
            
            // Scene view object selection
            if (mousePos.x >= 250 && mousePos.x <= 550 && mousePos.y >= 30 && mousePos.y <= 430)
            {
                selectedObject = nullptr;
                for (auto& obj : sceneObjects)
                {
                    auto transform = obj->GetComponent<Transform>();
                    if (transform)
                    {
                        Vector2 objPos = transform->GetPosition();
                        Vector2 objSize(32, 32); // Default click area
                        
                        auto sprite = obj->GetComponent<SpriteRenderer>();
                        if (sprite)
                            objSize = sprite->GetSize();
                            
                        if (IsPointInRect(mousePos, objPos - objSize/2, objSize))
                        {
                            SelectObject(obj.get());
                            break;
                        }
                    }
                }
            }
            
            showCreateDropdown = false;
        }
        
        // Right-click in scene view
        if (rightClick && !wasRightPressed)
        {
            if (mousePos.x >= 250 && mousePos.x <= 550 && mousePos.y >= 30 && mousePos.y <= 430)
            {
                GameObject* newObj = CreateEmpty("GameObject", mousePos);
                SelectObject(newObj);
            }
        }
        
        wasLeftPressed = leftClick;
        wasRightPressed = rightClick;
    }

    GameObject* UnityStyleEditor::CreateEmpty(const std::string& name, Vector2 position)
    {
        auto obj = std::make_unique<GameObject>(name);
        auto transform = obj->AddComponent<Transform>();
        transform->SetLocalPosition(position);
        
        GameObject* ptr = obj.get();
        sceneObjects.push_back(std::move(obj));
        
        DEBUG_LOG("Created Empty GameObject: " + name);
        return ptr;
    }

    GameObject* UnityStyleEditor::CreateSprite(const std::string& name, Vector2 position)
    {
        GameObject* obj = CreateEmpty(name, position);
        
        auto sprite = obj->AddComponent<SpriteRenderer>();
        sprite->SetColor(Color::White);
        sprite->SetSize(Vector2(64, 64));
        
        DEBUG_LOG("Created Sprite: " + name);
        return obj;
    }

    GameObject* UnityStyleEditor::CreateCube(const std::string& name, Vector2 position)
    {
        GameObject* obj = CreateEmpty(name, position);
        
        auto sprite = obj->AddComponent<SpriteRenderer>();
        sprite->SetColor(Color(0.7f, 0.7f, 0.7f));
        sprite->SetSize(Vector2(64, 64));
        
        DEBUG_LOG("Created Cube: " + name);
        return obj;
    }

    void UnityStyleEditor::SelectObject(GameObject* obj)
    {
        selectedObject = obj;
        if (obj)
            DEBUG_LOG("Selected: " + obj->GetName());
    }

    void UnityStyleEditor::DeleteSelected()
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

    bool UnityStyleEditor::IsPointInRect(Vector2 point, Vector2 pos, Vector2 size)
    {
        return point.x >= pos.x && point.x <= pos.x + size.x &&
               point.y >= pos.y && point.y <= pos.y + size.y;
    }
}