#pragma once
#include "Singleton.h"
#include "../Utils/Vector2.h"
#include <memory>

namespace AronEngine
{
    class AudioManager;
    class SoundBankManager;
    class RenderSystem;
    class EffectSystem;
    class VisualEffectSystem;
    class SceneManager;
    class ResourceManager;
    class TextureManager;
    class Renderer;

    class GameEngine : public Singleton<GameEngine>
    {
        friend class Singleton<GameEngine>;

    private:
        std::shared_ptr<AudioManager> audioManager;
        std::shared_ptr<SoundBankManager> soundBankManager;
        std::shared_ptr<RenderSystem> renderSystem;
        std::shared_ptr<EffectSystem> effectSystem;
        std::shared_ptr<VisualEffectSystem> visualEffectSystem;
        std::shared_ptr<SceneManager> sceneManager;
        std::shared_ptr<ResourceManager> resourceManager;
        std::shared_ptr<TextureManager> textureManager;
        std::shared_ptr<Renderer> renderer;

        bool isInitialized;
        bool isRunning;
        float targetFrameRate;
        float deltaTime;

    protected:
        GameEngine();
        virtual ~GameEngine();

    public:
        bool Initialize(void* windowHandle = nullptr);
        void Shutdown();
        
        void Update(float deltaTime);
        void Render();

        void Run();
        void Stop();

        // System accessors
        std::shared_ptr<AudioManager> GetAudioManager() const { return audioManager; }
        std::shared_ptr<SoundBankManager> GetSoundBankManager() const { return soundBankManager; }
        std::shared_ptr<RenderSystem> GetRenderSystem() const { return renderSystem; }
        std::shared_ptr<EffectSystem> GetEffectSystem() const { return effectSystem; }
        std::shared_ptr<VisualEffectSystem> GetVisualEffectSystem() const { return visualEffectSystem; }
        std::shared_ptr<SceneManager> GetSceneManager() const { return sceneManager; }
        std::shared_ptr<ResourceManager> GetResourceManager() const { return resourceManager; }
        std::shared_ptr<TextureManager> GetTextureManager() const { return textureManager; }
        std::shared_ptr<Renderer> GetRenderer() const { return renderer; }

        // Engine settings
        void SetTargetFrameRate(float fps) { targetFrameRate = fps; }
        float GetTargetFrameRate() const { return targetFrameRate; }
        
        float GetDeltaTime() const { return deltaTime; }
        bool IsRunning() const { return isRunning; }

        // Quick access methods for common operations
        void PlaySound(const std::string& bankName, const std::string& soundName);
        void PlayEffect(const std::string& effectName, const Vector2& position, float scale = 1.0f);
        void SetCameraPosition(const Vector2& position);
        void SetCameraBounds(float left, float right, float top, float bottom);

    private:
        bool InitializeSystems(void* windowHandle);
        void ShutdownSystems();
    };

    // Global convenience macros
    #define AUDIO_MANAGER GameEngine::GetInstance().GetAudioManager()
    #define SOUND_BANK_MANAGER GameEngine::GetInstance().GetSoundBankManager()
    #define RENDER_SYSTEM GameEngine::GetInstance().GetRenderSystem()
    #define EFFECT_SYSTEM GameEngine::GetInstance().GetEffectSystem()
    #define VISUAL_EFFECT_SYSTEM GameEngine::GetInstance().GetVisualEffectSystem()
    #define SCENE_MANAGER GameEngine::GetInstance().GetSceneManager()
    #define RESOURCE_MANAGER GameEngine::GetInstance().GetResourceManager()
    #define TEXTURE_MANAGER GameEngine::GetInstance().GetTextureManager()
    #define RENDERER GameEngine::GetInstance().GetRenderer()
}