#include "TestApp.h"
#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/EngineTime.h"
#include "Core/GameObject.h"
#include "Core/TextureManager.h"
#include "Managers/AudioManager.h"
#include "Editor/LightUnityEditor.h"
#include "Editor/KoreanStrings.h"
#include "Components/Transform.h"
#include "Components/SpriteRenderer.h"
#include "Components/AudioSource.h"
#include "Resources/AudioClip.h"
#include "Utils/Vector2.h"
#include "Utils/Color.h"
#include <memory>
#include <vector>
#include <windows.h>
#include <algorithm>

namespace AronEngine
{
    void TestApp::OnInit()
    {
        DEBUG_LOG("TestApp initialization completed - NEWARONENGINE with FMOD");
        
        // Try to load a test audio file using FMOD
        testAudioClip = AudioManager::GetInstance().LoadAudioClip("C:\\Windows\\Media\\ding.wav");
        if (testAudioClip)
        {
            DEBUG_LOG("Test audio loaded successfully with FMOD!");
            
            // Create a test GameObject with AudioSource component
            testObject = std::make_unique<GameObject>();
            testObject->SetName("AudioTestObject");
            
            auto audioSource = testObject->AddComponent<AudioSource>();
            if (audioSource)
            {
                audioSource->SetClip(testAudioClip);
                audioSource->SetLoop(false);
                audioSource->SetVolume(0.7f);
                DEBUG_LOG("AudioSource component created and configured");
            }
        }
        else
        {
            DEBUG_LOG("Test audio not found - using fallback audio file or system sounds");
        }
        
        // Initialize texture manager
        if (!TextureManager::GetInstance().Initialize(GetRenderer()->GetD2DContext()))
        {
            DEBUG_LOG("TextureManager initialization failed");
        }
        else
        {
            auto& texMgr = TextureManager::GetInstance();
            texMgr.CreateSolidColorTexture("default_white", 64, 64, Color::White);
            texMgr.CreateSolidColorTexture("default_red", 64, 64, Color::Red);
            texMgr.CreateSolidColorTexture("default_green", 64, 64, Color::Green);
            texMgr.CreateSolidColorTexture("default_blue", 64, 64, Color::Blue);
            texMgr.CreateSolidColorTexture("default_yellow", 64, 64, Color::Yellow);
            
            DEBUG_LOG("Default textures created successfully");
        }
        
        // Initialize light Unity-style editor
        editorSystem = &LightUnityEditor::GetInstance();
        if (!editorSystem->Initialize(GetWidth(), GetHeight()))
        {
            DEBUG_LOG("Failed to initialize LightUnityEditor");
        }
        showEditor = true;
        editorSystem->SetEnabled(true);
        
        // Initialize simple game state
        playerPos = Vector2(350, 400);
        playerVel = Vector2(0, 0);
        isJumping = false;
    }

    void TestApp::OnUpdate(float deltaTime)
    {
        if (GetInput()->GetKeyDown(KeyCode::Escape))
        {
            Quit();
        }

        // Toggle editor visibility
        if (GetInput()->GetKeyDown(KeyCode::E))
        {
            showEditor = !showEditor;
            if (editorSystem)
            {
                editorSystem->SetEnabled(showEditor);
                DEBUG_LOG("Editor toggled - showEditor: " + std::string(showEditor ? "true" : "false"));
            }
            else
            {
                DEBUG_LOG("Editor system is null - cannot toggle");
            }
        }

        if (GetInput()->GetKeyDown(KeyCode::F1))
        {
            DEBUG_LOG("F1 pressed - Debug info");
            DEBUG_LOG("FPS: " << GetTime()->GetFPS());
            DEBUG_LOG("Frame time: " << GetTime()->GetFrameTime() * 1000 << "ms");
        }

        // Simple player movement
        const float moveSpeed = 200.0f;
        const float jumpForce = 300.0f;
        const float gravity = 800.0f;
        
        // Horizontal movement
        if (GetInput()->GetKey(KeyCode::A) || GetInput()->GetKey(KeyCode::Left))
        {
            playerVel.x = -moveSpeed;
        }
        else if (GetInput()->GetKey(KeyCode::D) || GetInput()->GetKey(KeyCode::Right))
        {
            playerVel.x = moveSpeed;
        }
        else
        {
            playerVel.x *= 0.8f; // Friction
        }
        
        // Jumping with FMOD audio
        if ((GetInput()->GetKeyDown(KeyCode::W) || GetInput()->GetKeyDown(KeyCode::Up) || GetInput()->GetKeyDown(KeyCode::Space)) && !isJumping)
        {
            playerVel.y = -jumpForce;
            isJumping = true;
            
            // Play jump sound using FMOD
            if (testObject)
            {
                auto audioSource = testObject->GetComponent<AudioSource>();
                if (audioSource)
                {
                    audioSource->Play();
                    DEBUG_LOG("Playing jump sound via FMOD!");
                }
            }
            else
            {
                Beep(600, 50); // Fallback system beep
            }
        }
        
        // Apply gravity
        if (isJumping)
        {
            playerVel.y += gravity * deltaTime;
        }
        
        // Update position
        playerPos.x += playerVel.x * deltaTime;
        playerPos.y += playerVel.y * deltaTime;
        
        // Ground collision
        const float groundY = 400.0f;
        if (playerPos.y >= groundY)
        {
            playerPos.y = groundY;
            playerVel.y = 0;
            isJumping = false;
        }
        
        // Screen bounds
        if (playerPos.x < 0) playerPos.x = 0;
        if (playerPos.x > GetWidth() - 64) playerPos.x = GetWidth() - 64;

        // Test audio controls
        if (GetInput()->GetKeyDown(KeyCode::T))
        {
            if (testObject)
            {
                auto audioSource = testObject->GetComponent<AudioSource>();
                if (audioSource && testAudioClip)
                {
                    audioSource->Play();
                    DEBUG_LOG("Playing audio via FMOD (T key)!");
                }
            }
            else
            {
                Beep(800, 200);
                DEBUG_LOG("No FMOD audio - using fallback beep");
            }
        }
        
        if (GetInput()->GetKeyDown(KeyCode::M))
        {
            if (testObject)
            {
                auto audioSource = testObject->GetComponent<AudioSource>();
                if (audioSource)
                {
                    audioSource->Stop();
                    DEBUG_LOG("Audio stopped");
                }
            }
        }

        if (GetInput()->GetKeyDown(KeyCode::L))
        {
            if (testObject)
            {
                auto audioSource = testObject->GetComponent<AudioSource>();
                if (audioSource)
                {
                    bool currentLoop = audioSource->GetLoop();
                    audioSource->SetLoop(!currentLoop);
                    DEBUG_LOG("Loop mode: " + std::string(currentLoop ? "OFF" : "ON"));
                }
            }
        }
        
        // Update test object
        if (testObject)
        {
            testObject->Update(deltaTime);
        }
    }

    void TestApp::OnRender()
    {
        Renderer* renderer = GetRenderer();
        
        // Light Unity-style editor를 먼저 렌더링
        if (editorSystem && showEditor)
        {
            // 전체 화면을 밝은 회색으로 채우기
            renderer->FillRectangle(Vector2(0, 0), Vector2((float)GetWidth(), (float)GetHeight()), Color(0.95f, 0.95f, 0.95f));
            
            auto mousePos = GetInput()->GetMousePosition();
            bool leftClick = GetInput()->GetKey(KeyCode::MouseLeft);
            bool rightClick = GetInput()->GetKey(KeyCode::MouseRight);
            
            editorSystem->HandleInput(mousePos, leftClick, rightClick);
            
            // 키보드 입력 처리
            bool shift = GetInput()->GetKey(KeyCode::LeftShift) || GetInput()->GetKey(KeyCode::RightShift);
            bool ctrl = GetInput()->GetKey(KeyCode::LeftControl) || GetInput()->GetKey(KeyCode::RightControl);
            
            for (int key = 0; key < 256; key++)
            {
                if (GetInput()->GetKeyDown((KeyCode)key))
                {
                    editorSystem->HandleKeyInput(key, shift, ctrl);
                }
            }
            
            // Render scene objects
            auto& sceneObjects = editorSystem->GetSceneObjects();
            for (const auto& obj : sceneObjects)
            {
                RenderSceneObject(obj.get(), renderer);
            }
            
            // Render light editor UI
            editorSystem->Render(renderer);
            return; // 에디터만 표시하고 리턴
        }
        
        // 에디터가 꺼져있을 때만 게임 화면 표시
        // Instructions
        renderer->DrawText(L"NEWARONENGINE with FMOD Audio", Vector2(20, 20), Color::Yellow, L"Arial", 16.0f);
        renderer->DrawText(L"Controls:", Vector2(20, 50), Color::White, L"Arial", 14.0f);
        renderer->DrawText(L"WASD/Arrows: Move & Jump", Vector2(20, 70), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"T: Play Sound", Vector2(20, 90), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"M: Stop Sound", Vector2(20, 110), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"L: Toggle Loop", Vector2(20, 130), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"E: Toggle Editor", Vector2(20, 150), Color::Cyan, L"Arial", 12.0f);
        renderer->DrawText(L"ESC: Exit", Vector2(20, 170), Color::Cyan, L"Arial", 12.0f);
        
        // Simple character rendering
        Vector2 charPos = playerPos;
        
        // Head
        renderer->FillRectangle(charPos + Vector2(20, 0), Vector2(24, 24), Color(1.0f, 0.8f, 0.6f));
        
        // Eyes
        renderer->FillRectangle(charPos + Vector2(24, 6), Vector2(4, 4), Color::Black);
        renderer->FillRectangle(charPos + Vector2(32, 6), Vector2(4, 4), Color::Black);
        
        // Body
        renderer->FillRectangle(charPos + Vector2(16, 24), Vector2(32, 40), Color::Blue);
        
        // Arms
        renderer->FillRectangle(charPos + Vector2(8, 28), Vector2(12, 24), Color(1.0f, 0.8f, 0.6f));
        renderer->FillRectangle(charPos + Vector2(44, 28), Vector2(12, 24), Color(1.0f, 0.8f, 0.6f));
        
        // Legs
        renderer->FillRectangle(charPos + Vector2(20, 64), Vector2(10, 32), Color::Green);
        renderer->FillRectangle(charPos + Vector2(34, 64), Vector2(10, 32), Color::Green);
        
        // Draw ground line
        renderer->DrawLine(Vector2(0, 464), Vector2((float)GetWidth(), 464), Color::Gray, 2.0f);
        
        // Player status
        std::wstring posText = L"Player: (" + std::to_wstring((int)playerPos.x) + L", " + std::to_wstring((int)playerPos.y) + L")";
        renderer->DrawText(posText, Vector2(GetWidth() - 300, 50), Color::Cyan, L"Arial", 14.0f);
        
        std::wstring velText = L"Velocity: (" + std::to_wstring((int)playerVel.x) + L", " + std::to_wstring((int)playerVel.y) + L")";
        renderer->DrawText(velText, Vector2(GetWidth() - 300, 70), Color::Cyan, L"Arial", 14.0f);
        
        renderer->DrawText(isJumping ? L"Status: Jumping" : L"Status: On Ground", 
                          Vector2(GetWidth() - 300, 90), Color::Cyan, L"Arial", 14.0f);

        auto mousePos = GetInput()->GetMousePosition();
        renderer->FillCircle(mousePos, 5, Color::Magenta);
        
        std::wstring mousePosStr = L"Mouse: (" + std::to_wstring((int)mousePos.x) + L", " + std::to_wstring((int)mousePos.y) + L")";
        renderer->DrawText(mousePosStr, Vector2(mousePos.x + 10, mousePos.y - 20), Color::White, L"Arial", 12.0f);
        
        // Audio status
        if (testObject)
        {
            auto audioSource = testObject->GetComponent<AudioSource>();
            if (audioSource)
            {
                bool isPlaying = audioSource->IsPlaying();
                bool isLooping = audioSource->GetLoop();
                float volume = audioSource->GetVolume();
                
                renderer->DrawText(L"FMOD Audio Status:", Vector2(20, 200), Color::Green, L"Arial", 14.0f);
                renderer->DrawText(isPlaying ? L"Playing: YES" : L"Playing: NO", 
                                 Vector2(20, 220), isPlaying ? Color::Green : Color::Red, L"Arial", 12.0f);
                renderer->DrawText(isLooping ? L"Loop: YES" : L"Loop: NO", 
                                 Vector2(20, 240), isLooping ? Color::Green : Color::White, L"Arial", 12.0f);
                
                std::wstring volText = L"Volume: " + std::to_wstring((int)(volume * 100)) + L"%";
                renderer->DrawText(volText, Vector2(20, 260), Color::White, L"Arial", 12.0f);
            }
        }
        else
        {
            renderer->DrawText(L"FMOD Audio: Not Available", Vector2(20, 200), Color::Red, L"Arial", 14.0f);
        }
    }

    void TestApp::OnShutdown()
    {
        DEBUG_LOG("TestApp shutting down");
        
        if (testObject)
        {
            testObject.reset();
        }
        
        testAudioClip.reset();
        
        if (editorSystem)
        {
            editorSystem->Shutdown();
        }
        
        TextureManager::GetInstance().Shutdown();
    }

    void TestApp::RenderSceneObject(GameObject* gameObject, Renderer* renderer)
    {
        if (!gameObject) return;

        auto transform = gameObject->GetComponent<Transform>();
        if (!transform) return;
        
        Vector2 position = transform->GetPosition();
        auto spriteRenderer = gameObject->GetComponent<SpriteRenderer>();

        if (spriteRenderer)
        {
            // Render sprite
            Vector2 size = spriteRenderer->GetSize();
            renderer->FillRectangle(position, size, spriteRenderer->GetColor());
            
            // Unity-style selection outline
            if (editorSystem && editorSystem->GetSelectedObject() == gameObject)
            {
                Color selectionColor(1.0f, 0.6f, 0.0f); // Unity orange
                renderer->DrawRectangle(position - Vector2(2, 2), size + Vector2(4, 4), selectionColor, 3.0f);
                
                // Selection corners
                float cornerSize = 8.0f;
                Vector2 corners[4] = {
                    position - Vector2(cornerSize/2, cornerSize/2),
                    position + Vector2(size.x - cornerSize/2, -cornerSize/2),
                    position + Vector2(-cornerSize/2, size.y - cornerSize/2),
                    position + size - Vector2(cornerSize/2, cornerSize/2)
                };
                
                for (int i = 0; i < 4; i++)
                {
                    renderer->FillRectangle(corners[i], Vector2(cornerSize, cornerSize), selectionColor);
                }
            }
        }
        else
        {
            // Empty GameObject - clean gizmo
            Vector2 center = position;
            
            // Small cross gizmo (Unity-style)
            renderer->DrawLine(center - Vector2(8, 0), center + Vector2(8, 0), Color(0.8f, 0.2f, 0.2f), 2.0f); // X red
            renderer->DrawLine(center - Vector2(0, 8), center + Vector2(0, 8), Color(0.2f, 0.8f, 0.2f), 2.0f); // Y green
            
            // Selection highlight
            if (editorSystem && editorSystem->GetSelectedObject() == gameObject)
            {
                renderer->FillCircle(center, 12, Color(1.0f, 0.6f, 0.0f, 0.3f));
                renderer->DrawCircle(center, 12, Color(1.0f, 0.6f, 0.0f), 2.0f);
            }
        }
    }
}