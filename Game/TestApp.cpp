#include "TestApp.h"
#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/EngineTime.h"
#include "Core/GameObject.h"
#include "Core/TextureManager.h"
#include "Managers/SimpleAudioManager.h"
#include "Editor/ImGuiEditorSystem.h"
#include "Editor/KoreanStrings.h"
#include "Components/Transform.h"
#include "Components/SpriteRenderer.h"
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
        DEBUG_LOG("TestApp initialization completed");
        
        // Try to load a test audio file (you can create a simple WAV or use Windows system sound)
        testAudio = SimpleAudioManager::GetInstance().LoadAudioClip("C:\\Windows\\Media\\ding.wav");
        audioLoaded = (testAudio != nullptr && testAudio->IsLoaded());
        
        if (audioLoaded)
        {
            DEBUG_LOG("Test audio loaded successfully!");
        }
        else
        {
            DEBUG_LOG("Test audio not found - please place test WAV file");
        }
        
        // Initialize texture manager
        if (!TextureManager::GetInstance().Initialize(GetRenderer()->GetD2DContext()))
        {
            DEBUG_LOG("TextureManager initialization failed");
        }
        else
        {
            // 기본 텍스처들 생성
            auto& texMgr = TextureManager::GetInstance();
            texMgr.CreateSolidColorTexture("default_white", 64, 64, Color::White);
            texMgr.CreateSolidColorTexture("default_red", 64, 64, Color::Red);
            texMgr.CreateSolidColorTexture("default_green", 64, 64, Color::Green);
            texMgr.CreateSolidColorTexture("default_blue", 64, 64, Color::Blue);
            texMgr.CreateSolidColorTexture("default_yellow", 64, 64, Color::Yellow);
            
            DEBUG_LOG("Default textures created successfully");
        }
        
        // Initialize editor system
        editorSystem = &ImGuiEditorSystem::GetInstance();
        if (!editorSystem->Initialize(GetWindowHandle(), GetRenderer()))
        {
            DEBUG_LOG("ImGui Editor System initialization failed");
            editorSystem = nullptr;
        }
        showEditor = true;
        
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
        
        // Jumping
        if ((GetInput()->GetKeyDown(KeyCode::W) || GetInput()->GetKeyDown(KeyCode::Up) || GetInput()->GetKeyDown(KeyCode::Space)) && !isJumping)
        {
            playerVel.y = -jumpForce;
            isJumping = true;
            
            // Play jump sound
            Beep(600, 50);
        }
        
        // Apply gravity
        if (isJumping)
        {
            playerVel.y += gravity * deltaTime;
        }
        
        // Update position
        playerPos.x += playerVel.x * deltaTime;
        playerPos.y += playerVel.y * deltaTime;
        
        // Ground collision (simple)
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

        // Test audio playback  
        if (GetInput()->GetKeyDown(KeyCode::T))
        {
            DEBUG_LOG("Playing test audio (Space key pressed)");
            
            // Try multiple Windows system sounds
            bool soundPlayed = false;
            
            // Try Windows system beep first (simplest)
            if (Beep(800, 200)) // 800Hz for 200ms
            {
                DEBUG_LOG("System beep played!");
                soundPlayed = true;
            }
            
            // Try Windows system error sound
            if (!soundPlayed)
            {
                if (MessageBeep(MB_OK))
                {
                    DEBUG_LOG("System message beep played!");
                    soundPlayed = true;
                }
            }
            
            // Try PlaySound with Windows system sounds
            if (!soundPlayed)
            {
                // Try different Windows system sound paths
                std::vector<std::string> soundPaths = {
                    "C:\\Windows\\Media\\ding.wav",
                    "C:\\Windows\\Media\\chord.wav", 
                    "C:\\Windows\\Media\\Windows Ding.wav",
                    "C:\\Windows\\Media\\notify.wav"
                };
                
                for (const auto& path : soundPaths)
                {
                    if (SimpleAudioManager::GetInstance().PlaySound(path, true))
                    {
                        DEBUG_LOG("Played sound: " + path);
                        soundPlayed = true;
                        break;
                    }
                }
            }
            
            if (!soundPlayed)
            {
                DEBUG_LOG("No sound could be played - check Windows Media folder");
                // Last resort: Windows default beep
                MessageBeep(0xFFFFFFFF);
            }
        }

        if (GetInput()->GetKeyDown(KeyCode::M))
        {
            // Test stopping all sounds
            SimpleAudioManager::GetInstance().StopAllSounds();
            DEBUG_LOG("All sounds stopped");
        }

        if (GetInput()->GetKeyDown(KeyCode::L))
        {
            DEBUG_LOG("L key pressed - Testing direct PlaySound call");
            
            // Direct PlaySound API call for testing
            BOOL result = PlaySoundA("SystemDefault", nullptr, SND_ALIAS | SND_ASYNC);
            if (result)
            {
                DEBUG_LOG("Direct PlaySound successful!");
            }
            else
            {
                DEBUG_LOG("Direct PlaySound failed, error: " + std::to_string(GetLastError()));
                
                // Try system sounds
                PlaySoundA("SystemQuestion", nullptr, SND_ALIAS | SND_ASYNC);
            }
        }
        
        if (GetInput()->GetKeyDown(KeyCode::B))
        {
            DEBUG_LOG("B key pressed - Testing Beep");
            
            // Test different beep frequencies
            Beep(1000, 100); // 1000Hz for 100ms
            Sleep(150);
            Beep(1500, 100); // 1500Hz for 100ms  
            Sleep(150);
            Beep(2000, 100); // 2000Hz for 100ms
        }
    }

    void TestApp::OnRender()
    {
        auto renderer = GetRenderer();
        
        // Begin ImGui frame if editor is enabled
        if (editorSystem && showEditor)
        {
            editorSystem->BeginFrame();
        }
        
        renderer->DrawText(L"AronEngine", Vector2(50, 50), Color::White, L"Arial", 32.0f);
        // 한글 문자열을 wstring으로 변환하는 유틸리티
        auto toWString = [](const char* utf8str) {
            int wchars_num = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, NULL, 0);
            wchar_t* wstr = new wchar_t[wchars_num];
            MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, wstr, wchars_num);
            std::wstring result(wstr);
            delete[] wstr;
            return result;
        };

        renderer->DrawText(L"F1 - Debug Info", Vector2(50, 100), Color::Gray, L"Arial", 16.0f);
        renderer->DrawText(L"ESC - Exit", Vector2(50, 120), Color::Gray, L"Arial", 16.0f);
        
        // Game controls
        renderer->DrawText(L"WASD/Arrows: Move Player", Vector2(50, 150), Color::Green, L"Arial", 16.0f);
        renderer->DrawText(L"SPACE/W/UP: Jump", Vector2(50, 170), Color::Green, L"Arial", 16.0f);
        renderer->DrawText(L"E: Toggle Editor", Vector2(50, 190), Color::Green, L"Arial", 16.0f);
        renderer->DrawText(L"T: Test Audio", Vector2(50, 210), Color::Green, L"Arial", 16.0f);
        renderer->DrawText(L"B: Test Beep", Vector2(50, 230), Color::Green, L"Arial", 16.0f);
        
        if (audioLoaded)
        {
            renderer->DrawText(L"Audio System Ready!", Vector2(50, 260), Color::Yellow, L"Arial", 16.0f);
        }
        else
        {
            renderer->DrawText(L"Using System Sound", Vector2(50, 260), Color::Yellow, L"Arial", 16.0f);
        }
        
        // Editor status
        std::wstring editorStatus = showEditor ? L"Editor: ON (Press E to toggle)" : L"Editor: OFF (Press E to toggle)";
        renderer->DrawText(editorStatus, Vector2(50, 280), Color::Cyan, L"Arial", 16.0f);

        // Draw some test sprites/objects
        renderer->FillRectangle(Vector2(400, 300), Vector2(64, 64), Color::Red);     // Simple red sprite
        renderer->FillRectangle(Vector2(500, 300), Vector2(64, 64), Color::Green);   // Simple green sprite
        renderer->FillRectangle(Vector2(600, 300), Vector2(64, 64), Color::Blue);    // Simple blue sprite
        
        // Draw the player character at current position
        Vector2 charPos = playerPos;
        
        // Head
        renderer->FillRectangle(charPos + Vector2(20, 0), Vector2(24, 24), Color(1.0f, 0.8f, 0.6f)); // Skin color
        
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
        
        // Test animation - moving circle
        static float animTime = 0.0f;
        animTime += GetTime()->GetDeltaTime();
        float xOffset = sin(animTime * 2.0f) * 100.0f;
        renderer->FillCircle(Vector2(700 + xOffset, 400), 20, Color::Yellow);
        
        // Draw ground line
        renderer->DrawLine(Vector2(0, 464), Vector2((float)GetWidth(), 464), Color::Gray, 2.0f);
        
        // Player status
        std::wstring posText = std::wstring(L"Player: ") + L"(" + std::to_wstring((int)playerPos.x) + L", " + std::to_wstring((int)playerPos.y) + L")";
        renderer->DrawText(posText, Vector2(GetWidth() - 300, 50), Color::Cyan, L"Arial", 14.0f);
        
        std::wstring velText = std::wstring(L"Velocity: ") + L"(" + std::to_wstring((int)playerVel.x) + L", " + std::to_wstring((int)playerVel.y) + L")";
        renderer->DrawText(velText, Vector2(GetWidth() - 300, 70), Color::Cyan, L"Arial", 14.0f);
        
        renderer->DrawText(isJumping ? L"Status: Jumping" : L"Status: On Ground", 
                          Vector2(GetWidth() - 300, 90), Color::Cyan, L"Arial", 14.0f);

        auto mousePos = GetInput()->GetMousePosition();
        renderer->FillCircle(mousePos, 5, Color::Magenta);
        
        std::wstring mousePosStr = std::wstring(L"Mouse: ") + L"(" + std::to_wstring((int)mousePos.x) + L", " + std::to_wstring((int)mousePos.y) + L")";
        renderer->DrawText(mousePosStr, Vector2(mousePos.x + 10, mousePos.y - 20), Color::White, L"Arial", 12.0f);
        
        // Render GameObjects created in editor
        if (editorSystem && showEditor)
        {
            auto& gameObjects = editorSystem->GetGameObjects();
            for (const auto& gameObject : gameObjects)
            {
                if (gameObject->IsActive())
                {
                    RenderGameObject(gameObject.get(), renderer);
                }
            }
        }
        
        // Render ImGui editor if enabled
        if (editorSystem && showEditor)
        {
            editorSystem->Render();
            editorSystem->EndFrame();
        }
    }

    void TestApp::OnShutdown()
    {
        if (editorSystem)
        {
            editorSystem->Shutdown();
        }
        DEBUG_LOG("TestApp shutting down");
    }

    void TestApp::RenderGameObject(GameObject* gameObject, Renderer* renderer)
    {
        if (!gameObject || !gameObject->IsActive()) return;
        
        Transform* transform = gameObject->GetComponent<Transform>();
        SpriteRenderer* spriteRenderer = gameObject->GetComponent<SpriteRenderer>();
        
        if (!transform) return;
        
        Vector2 position = transform->GetWorldPosition();
        Vector2 scale = transform->GetScale();
        float rotation = transform->GetRotation();
        
        if (spriteRenderer)
        {
            Vector2 size = spriteRenderer->GetSize();
            Color color = spriteRenderer->GetColor();
            Vector2 finalSize = Vector2(size.x * scale.x, size.y * scale.y);
            
            std::shared_ptr<Sprite> gameSprite = spriteRenderer->GetSprite();
            if (gameSprite && gameSprite->texture)
            {
                Vector2 renderPos = position - finalSize * 0.5f;
                renderer->DrawTexture(gameSprite->texture, renderPos, finalSize, color);
            }
            else
            {
                if (gameObject->GetName().find("Circle") != std::string::npos)
                {
                    float radius = (std::min)(finalSize.x, finalSize.y) * 0.5f;
                    renderer->FillCircle(position, radius, color);
                }
                else
                {
                    renderer->FillRectangle(position - finalSize * 0.5f, finalSize, color);
                }
            }
        }
    }
}
