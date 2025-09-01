#include "../Engine/Core/GameEngine.h"
#include "../Engine/Core/GameObject.h"
#include "../Engine/Components/Transform.h"
#include "../Engine/Components/SpriteRenderer.h"
#include "../Engine/Components/SoundComponent.h"
#include "../Engine/Components/EffectComponent.h"
#include "../Engine/Components/AABB.h"

using namespace AronEngine;

class AdvancedDemo
{
public:
    void Initialize()
    {
        // Initialize the game engine
        GameEngine::GetInstance().Initialize();
        
        // Load sound banks
        SOUND_BANK_MANAGER->LoadSoundBank("Combat", "Assets/Audio/CombatSounds.bank");
        SOUND_BANK_MANAGER->LoadSoundBank("Environment", "Assets/Audio/EnvironmentSounds.bank");
        
        // Load effect definitions
        EFFECT_SYSTEM->LoadEffectDefinition("Explosion", "Assets/Effects/Explosion.effect");
        EFFECT_SYSTEM->LoadEffectDefinition("Smoke", "Assets/Effects/Smoke.effect");
        EFFECT_SYSTEM->LoadEffectDefinition("Magic", "Assets/Effects/Magic.effect");
        
        // Setup camera culling
        RENDER_SYSTEM->SetCameraBounds(-500, 1500, -400, 1200);
        RENDER_SYSTEM->SetCullingEnabled(true);
        
        CreateDemoObjects();
        
        GameEngine::GetInstance().Run();
    }

    void CreateDemoObjects()
    {
        // Create player with advanced sound component
        auto player = std::make_shared<GameObject>("Player");
        auto playerTransform = player->AddComponent<Transform>();
        playerTransform->SetPosition(Vector2(500, 400));
        
        auto playerSprite = player->AddComponent<SpriteRenderer>();
        playerSprite->LoadSpriteFromFile("Assets/Textures/Player.png");
        
        auto playerSound = player->AddComponent<SoundComponent>();
        playerSound->SetSoundBank("Combat", "PlayerFootsteps");
        playerSound->SetLoop(true);
        playerSound->SetVolume(0.7f);
        playerSound->SetAutoPlay(true);
        
        auto playerAABB = player->AddComponent<AABB>();
        playerAABB->SetSize(Vector2(64, 64));
        playerAABB->SetIsTrigger(false);

        // Create explosion effect demo
        CreateExplosionDemo();
        
        // Create ambient sound demo
        CreateAmbientSoundDemo();
        
        // Create dynamic effect demo
        CreateDynamicEffectDemo();
    }

    void CreateExplosionDemo()
    {
        // Create explosion trigger
        auto explosionTrigger = std::make_shared<GameObject>("ExplosionTrigger");
        auto transform = explosionTrigger->AddComponent<Transform>();
        transform->SetPosition(Vector2(300, 300));
        
        auto aabb = explosionTrigger->AddComponent<AABB>();
        aabb->SetSize(Vector2(50, 50));
        aabb->SetIsTrigger(true);
        
        // When triggered, create explosion effect
        // (In a real implementation, you'd have an event system for this)
    }

    void CreateAmbientSoundDemo()
    {
        // Background ambient sound
        auto ambientSound = std::make_shared<GameObject>("AmbientSound");
        auto soundComp = ambientSound->AddComponent<SoundComponent>();
        soundComp->SetSoundBank("Environment", "ForestAmbient");
        soundComp->SetLoop(true);
        soundComp->SetVolume(0.3f);
        soundComp->SetAutoPlay(true);
        
        // Fade in the ambient sound
        soundComp->FadeIn(2.0f);
    }

    void CreateDynamicEffectDemo()
    {
        // Create magic effect with dynamic scaling
        auto magicEffect = std::make_shared<GameObject>("MagicEffect");
        auto transform = magicEffect->AddComponent<Transform>();
        transform->SetPosition(Vector2(700, 300));
        
        auto sprite = magicEffect->AddComponent<SpriteRenderer>();
        sprite->LoadSpriteFromFile("Assets/Effects/MagicCircle.png");
        
        auto effect = magicEffect->AddComponent<EffectComponent>();
        effect->SetEffectName("Magic");
        effect->SetDuration(3.0f);
        effect->SetLoop(true);
        effect->SetScaleOverTime(true, Vector2(0.5f, 0.5f), Vector2(2.0f, 2.0f));
        effect->SetAlphaOverTime(true, 1.0f, 0.3f);
        effect->Play();
    }

    void Update(float deltaTime)
    {
        GameEngine::GetInstance().Update(deltaTime);
        
        // Demo-specific update logic
        HandleInput();
        UpdateEffects(deltaTime);
    }

    void Render()
    {
        GameEngine::GetInstance().Render();
        
        // Display performance info
        if (RENDER_SYSTEM->IsCullingEnabled())
        {
            int culled = RENDER_SYSTEM->GetCulledObjectCount();
            int total = RENDER_SYSTEM->GetTotalObjectCount();
            float ratio = RENDER_SYSTEM->GetCullingRatio();
            
            // DEBUG_LOG("Culling: " + std::to_string(culled) + "/" + std::to_string(total) + " (" + std::to_string(ratio * 100) + "%)");
        }
    }

private:
    void HandleInput()
    {
        // Demo input handling
        // Spacebar to create explosion effect
        static bool spacePressed = false;
        // if (Input::GetKeyDown(KEY_SPACE) && !spacePressed)
        // {
        //     GameEngine::GetInstance().PlayEffect("Explosion", Vector2(400, 400), 1.5f);
        //     GameEngine::GetInstance().PlaySound("Combat", "ExplosionSound");
        //     spacePressed = true;
        // }
        // else if (!Input::GetKeyDown(KEY_SPACE))
        // {
        //     spacePressed = false;
        // }
    }

    void UpdateEffects(float deltaTime)
    {
        // Update any custom effect logic
        static float effectTimer = 0.0f;
        effectTimer += deltaTime;
        
        // Create periodic smoke effects
        if (effectTimer >= 2.0f)
        {
            EFFECT_SYSTEM->CreateSmokeEffect("Smoke", Vector2(600, 200), 4.0f);
            effectTimer = 0.0f;
        }
    }
};

// Usage example in main.cpp:
/*
int main()
{
    AdvancedDemo demo;
    demo.Initialize();
    
    // Game loop
    while (GameEngine::GetInstance().IsRunning())
    {
        float deltaTime = CalculateDeltaTime(); // Your time calculation
        
        demo.Update(deltaTime);
        demo.Render();
    }
    
    GameEngine::GetInstance().Shutdown();
    return 0;
}
*/