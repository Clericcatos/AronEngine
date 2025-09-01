#include "framework.h"
#include "GameEngine.h"
#include "../Managers/AudioManager.h"
#include "../Managers/SoundBank.h"
#include "../Managers/SceneManager.h"
#include "../Managers/ResourceManager.h"
#include "../Core/TextureManager.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/EffectSystem.h"
#include "../Systems/VisualEffectSystem.h"
#include "Renderer.h"

namespace AronEngine
{
    GameEngine::GameEngine()
        : isInitialized(false)
        , isRunning(false)
        , targetFrameRate(60.0f)
        , deltaTime(0.0f)
    {
    }

    GameEngine::~GameEngine()
    {
        Shutdown();
    }

    bool GameEngine::Initialize(void* windowHandle)
    {
        if (isInitialized)
        {
            DEBUG_LOG("GameEngine already initialized");
            return true;
        }

        DEBUG_LOG("Initializing GameEngine...");

        if (!InitializeSystems(windowHandle))
        {
            DEBUG_LOG("Failed to initialize engine systems");
            Shutdown();
            return false;
        }

        isInitialized = true;
        DEBUG_LOG("GameEngine initialized successfully");
        return true;
    }

    void GameEngine::Shutdown()
    {
        if (!isInitialized) return;

        DEBUG_LOG("Shutting down GameEngine...");
        
        isRunning = false;
        ShutdownSystems();
        
        isInitialized = false;
        DEBUG_LOG("GameEngine shutdown complete");
    }

    void GameEngine::Update(float dt)
    {
        if (!isInitialized || !isRunning) return;

        deltaTime = dt;

        if (renderSystem)
        {
            renderSystem->Update(deltaTime);
        }

        if (effectSystem)
        {
            effectSystem->Update(deltaTime);
        }

        if (audioManager)
        {
            audioManager->Update();
        }

        if (sceneManager)
        {
            sceneManager->Update(deltaTime);
        }
    }

    void GameEngine::Render()
    {
        if (!isInitialized || !isRunning || !renderer || !renderSystem) return;

        renderer->BeginDraw();
        renderer->Clear(Color(0.2f, 0.3f, 0.4f, 1.0f));
        
        renderSystem->Render(renderer.get());
        
        if (effectSystem)
        {
            effectSystem->Render();
        }
        
        renderer->EndDraw();
    }

    void GameEngine::Run()
    {
        if (!isInitialized)
        {
            DEBUG_LOG("Cannot run: GameEngine not initialized");
            return;
        }

        isRunning = true;
        DEBUG_LOG("GameEngine started running");
    }

    void GameEngine::Stop()
    {
        isRunning = false;
        DEBUG_LOG("GameEngine stopped");
    }

    void GameEngine::PlaySound(const std::string& bankName, const std::string& soundName)
    {
        if (soundBankManager)
        {
            soundBankManager->PlaySound(bankName, soundName);
        }
    }

    void GameEngine::PlayEffect(const std::string& effectName, const Vector2& position, float scale)
    {
        if (effectSystem)
        {
            effectSystem->CreateEffect(effectName, position, scale);
        }
    }

    void GameEngine::SetCameraPosition(const Vector2& position)
    {
        DEBUG_LOG("Camera position set to: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
    }

    void GameEngine::SetCameraBounds(float left, float right, float top, float bottom)
    {
        if (renderSystem)
        {
            renderSystem->SetCameraBounds(left, right, top, bottom);
            renderSystem->SetCullingEnabled(true);
        }
    }

    bool GameEngine::InitializeSystems(void* windowHandle)
    {
        // Initialize core managers
        audioManager = std::shared_ptr<AudioManager>(&AudioManager::GetInstance(), [](AudioManager*){});
        if (!audioManager->Initialize(windowHandle))
        {
            DEBUG_LOG("Failed to initialize AudioManager");
            return false;
        }

        soundBankManager = std::make_shared<SoundBankManager>();

        textureManager = std::shared_ptr<TextureManager>(&TextureManager::GetInstance(), [](TextureManager*){});
        
        resourceManager = std::make_shared<ResourceManager>();
        
        sceneManager = std::make_shared<SceneManager>();

        // Initialize render system
        renderSystem = std::make_shared<RenderSystem>();
        
        // Initialize effect systems
        effectSystem = std::make_shared<EffectSystem>();
        renderSystem->SetEffectSystem(effectSystem);
        
        visualEffectSystem = std::make_shared<VisualEffectSystem>();

        // Initialize renderer (would need actual window handle and size)
        // renderer = std::make_shared<Renderer>();
        
        DEBUG_LOG("All engine systems initialized successfully");
        return true;
    }

    void GameEngine::ShutdownSystems()
    {
        visualEffectSystem.reset();
        effectSystem.reset();
        renderSystem.reset();
        renderer.reset();
        
        if (soundBankManager)
        {
            soundBankManager->UnloadAllSoundBanks();
            soundBankManager.reset();
        }
        
        if (audioManager)
        {
            audioManager->Shutdown();
            audioManager.reset();
        }
        
        sceneManager.reset();
        resourceManager.reset();
        textureManager.reset();
    }
}