#include "framework.h"
#include "EffectSystem.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include "../Components/SpriteRenderer.h"
#include "../Resources/AnimationClip.h"
#include <fstream>
#include <algorithm>

// Forward declaration to avoid circular dependency
namespace AronEngine
{
    class EffectComponent;
}

namespace AronEngine
{
    void EffectSystem::Update(float deltaTime)
    {
        UpdateEffectLifetime(deltaTime);
        CleanupDestroyedEffects();
    }

    void EffectSystem::Render()
    {
        // Effects are rendered by the regular RenderSystem through SpriteRenderer components
    }

    bool EffectSystem::LoadEffectDefinition(const std::string& effectName, const std::string& definitionPath)
    {
        EffectDefinition definition;
        definition.name = effectName;

        if (!ParseEffectDefinitionFile(definitionPath, definition))
        {
            DEBUG_LOG("Failed to load effect definition: " + effectName);
            return false;
        }

        effectDefinitions[effectName] = definition;
        DEBUG_LOG("Effect definition loaded: " + effectName);
        return true;
    }

    void EffectSystem::UnloadEffectDefinition(const std::string& effectName)
    {
        auto it = effectDefinitions.find(effectName);
        if (it != effectDefinitions.end())
        {
            effectDefinitions.erase(it);
            DEBUG_LOG("Effect definition unloaded: " + effectName);
        }
    }

    std::shared_ptr<GameObject> EffectSystem::CreateEffect(const std::string& effectName, const Vector2& position, float scale)
    {
        auto it = effectDefinitions.find(effectName);
        if (it == effectDefinitions.end())
        {
            DEBUG_LOG("Effect definition not found: " + effectName);
            return nullptr;
        }

        const EffectDefinition& def = it->second;
        Vector2 scaledSize = def.defaultSize * scale;
        
        return CreateEffect(effectName, position, scaledSize);
    }

    std::shared_ptr<GameObject> EffectSystem::CreateEffect(const std::string& effectName, const Vector2& position, const Vector2& size)
    {
        auto it = effectDefinitions.find(effectName);
        if (it == effectDefinitions.end())
        {
            DEBUG_LOG("Effect definition not found: " + effectName);
            return nullptr;
        }

        const EffectDefinition& def = it->second;

        auto effectObject = std::make_shared<GameObject>("Effect_" + effectName);
        
        auto transform = effectObject->AddComponent<Transform>();
        transform->SetPosition(position);
        
        auto spriteRenderer = effectObject->AddComponent<SpriteRenderer>();
        spriteRenderer->LoadSpriteFromFile(def.texturePath);
        spriteRenderer->SetSize(size);
        
        // Create basic effect without EffectComponent for now
        EffectInstance instance;
        instance.gameObject = effectObject;
        instance.effectComponent = nullptr;
        instance.lifetime = def.autoDestroy ? def.defaultDuration : -1.0f;
        instance.currentTime = 0.0f;
        instance.autoDestroy = def.autoDestroy;
        instance.effectName = effectName;

        activeEffects.push_back(instance);

        DEBUG_LOG("Effect created: " + effectName + " at (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
        return effectObject;
    }

    void EffectSystem::CreateEffectOneShot(const std::string& effectName, const Vector2& position, float duration)
    {
        auto effectObject = CreateEffect(effectName, position, 1.0f);
        if (!effectObject) return;

        DEBUG_LOG("One-shot effect created: " + effectName);
    }

    void EffectSystem::CreateExplosionEffect(const std::string& effectName, const Vector2& position, float scale)
    {
        auto effectObject = CreateEffect(effectName, position, scale);
        if (!effectObject) return;

        DEBUG_LOG("Explosion effect created: " + effectName);
    }

    void EffectSystem::CreateSmokeEffect(const std::string& effectName, const Vector2& position, float duration)
    {
        auto effectObject = CreateEffect(effectName, position, 1.0f);
        if (!effectObject) return;

        DEBUG_LOG("Smoke effect created: " + effectName);
    }

    void EffectSystem::DestroyEffect(std::shared_ptr<GameObject> effectObject)
    {
        auto it = std::find_if(activeEffects.begin(), activeEffects.end(),
            [&effectObject](const EffectInstance& instance) {
                return instance.gameObject == effectObject;
            });

        if (it != activeEffects.end())
        {
            activeEffects.erase(it);
            DEBUG_LOG("Effect destroyed manually");
        }
    }

    void EffectSystem::DestroyAllEffects()
    {
        activeEffects.clear();
        DEBUG_LOG("All effects destroyed");
    }

    void EffectSystem::RegisterEffectComponent(EffectComponent* effectComponent)
    {
        // Effects are automatically registered when created through CreateEffect
    }

    void EffectSystem::UnregisterEffectComponent(EffectComponent* effectComponent)
    {
        auto it = std::find_if(activeEffects.begin(), activeEffects.end(),
            [effectComponent](const EffectInstance& instance) {
                return instance.effectComponent == effectComponent;
            });

        if (it != activeEffects.end())
        {
            activeEffects.erase(it);
        }
    }

    bool EffectSystem::ParseEffectDefinitionFile(const std::string& filePath, EffectDefinition& definition)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open effect definition file: " + filePath);
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;

            if (line.substr(0, 9) == "TEXTURE:")
            {
                definition.texturePath = line.substr(9);
            }
            else if (line.substr(0, 10) == "ANIMATION:")
            {
                definition.animationClipPath = line.substr(10);
            }
            else if (line.substr(0, 6) == "SCALE:")
            {
                definition.defaultScale = std::stof(line.substr(6));
            }
            else if (line.substr(0, 9) == "DURATION:")
            {
                definition.defaultDuration = std::stof(line.substr(9));
            }
            else if (line.substr(0, 5) == "LOOP:")
            {
                definition.loop = (line.substr(5) == "true");
            }
            else if (line.substr(0, 12) == "AUTODESTROY:")
            {
                definition.autoDestroy = (line.substr(12) == "true");
            }
            else if (line.substr(0, 5) == "SIZE:")
            {
                std::string sizeStr = line.substr(5);
                size_t comma = sizeStr.find(',');
                if (comma != std::string::npos)
                {
                    definition.defaultSize.x = std::stof(sizeStr.substr(0, comma));
                    definition.defaultSize.y = std::stof(sizeStr.substr(comma + 1));
                }
            }
        }

        return true;
    }

    void EffectSystem::UpdateEffectLifetime(float deltaTime)
    {
        for (auto& effect : activeEffects)
        {
            if (effect.autoDestroy && effect.lifetime > 0.0f)
            {
                effect.currentTime += deltaTime;
                
                if (effect.currentTime >= effect.lifetime)
                {
                    if (effect.gameObject)
                    {
                        effect.gameObject->SetActive(false);
                    }
                }
            }
        }
    }

    void EffectSystem::CleanupDestroyedEffects()
    {
        activeEffects.erase(
            std::remove_if(activeEffects.begin(), activeEffects.end(),
                [](const EffectInstance& effect) {
                    return !effect.gameObject || !effect.gameObject->IsActive() ||
                           (effect.autoDestroy && effect.lifetime > 0.0f && effect.currentTime >= effect.lifetime);
                }),
            activeEffects.end());
    }
}