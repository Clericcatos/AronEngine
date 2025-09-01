#pragma once
#include "../Core/framework.h"
#include "../Utils/Vector2.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace AronEngine
{
    class GameObject;
    class EffectComponent;
    class AnimationClip;

    struct EffectInstance
    {
        std::shared_ptr<GameObject> gameObject;
        EffectComponent* effectComponent;
        float lifetime;
        float currentTime;
        bool autoDestroy;
        std::string effectName;

        EffectInstance() : lifetime(0.0f), currentTime(0.0f), autoDestroy(true) {}
    };

    struct EffectDefinition
    {
        std::string name;
        std::string animationClipPath;
        std::string texturePath;
        float defaultScale;
        float defaultDuration;
        bool loop;
        bool autoDestroy;
        Vector2 defaultSize;

        EffectDefinition() : defaultScale(1.0f), defaultDuration(1.0f), loop(false), autoDestroy(true), defaultSize(100, 100) {}
    };

    class EffectSystem
    {
    private:
        std::vector<EffectInstance> activeEffects;
        std::unordered_map<std::string, EffectDefinition> effectDefinitions;

    public:
        EffectSystem() = default;
        ~EffectSystem() = default;

        void Update(float deltaTime);
        void Render();

        bool LoadEffectDefinition(const std::string& effectName, const std::string& definitionPath);
        void UnloadEffectDefinition(const std::string& effectName);

        std::shared_ptr<GameObject> CreateEffect(const std::string& effectName, const Vector2& position, float scale = 1.0f);
        std::shared_ptr<GameObject> CreateEffect(const std::string& effectName, const Vector2& position, const Vector2& size);
        
        void CreateEffectOneShot(const std::string& effectName, const Vector2& position, float duration = -1.0f);
        void CreateExplosionEffect(const std::string& effectName, const Vector2& position, float scale = 1.0f);
        void CreateSmokeEffect(const std::string& effectName, const Vector2& position, float duration = 5.0f);

        void DestroyEffect(std::shared_ptr<GameObject> effectObject);
        void DestroyAllEffects();

        void RegisterEffectComponent(EffectComponent* effectComponent);
        void UnregisterEffectComponent(EffectComponent* effectComponent);

        size_t GetActiveEffectCount() const { return activeEffects.size(); }
        const std::vector<EffectInstance>& GetActiveEffects() const { return activeEffects; }

    private:
        bool ParseEffectDefinitionFile(const std::string& filePath, EffectDefinition& definition);
        void UpdateEffectLifetime(float deltaTime);
        void CleanupDestroyedEffects();
    };
}