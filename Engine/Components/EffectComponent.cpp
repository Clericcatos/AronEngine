#include "framework.h"
#include "EffectComponent.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include "../Components/SpriteRenderer.h"
#include "../Resources/AnimationClip.h"
#include <algorithm>
#include <cmath>

namespace AronEngine
{
    EffectComponent::EffectComponent()
        : Component("EffectComponent")
        , animationClip(nullptr)
        , spriteRenderer(nullptr)
        , state(EffectState::Stopped)
        , duration(1.0f)
        , currentTime(0.0f)
        , playbackSpeed(1.0f)
        , startScale(1.0f, 1.0f)
        , endScale(1.0f, 1.0f)
        , currentScale(1.0f, 1.0f)
        , startAlpha(1.0f)
        , endAlpha(1.0f)
        , currentAlpha(1.0f)
        , loop(false)
        , autoDestroy(true)
        , scaleOverTime(false)
        , alphaOverTime(false)
        , scaleEaseType(0.0f)
        , alphaEaseType(0.0f)
    {
    }

    void EffectComponent::Start()
    {
        Component::Start();
        
        spriteRenderer = GetGameObject()->GetComponent<SpriteRenderer>();
        if (!spriteRenderer)
        {
            DEBUG_LOG("EffectComponent requires SpriteRenderer component");
        }
    }

    void EffectComponent::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        if (state == EffectState::Playing)
        {
            UpdateAnimation(deltaTime);
            UpdateScaling(deltaTime);
            UpdateAlpha(deltaTime);
            ApplyVisualChanges();
        }
    }

    void EffectComponent::OnDestroy()
    {
        Stop();
        Component::OnDestroy();
    }

    void EffectComponent::Play()
    {
        if (state == EffectState::Playing) return;
        
        state = EffectState::Playing;
        currentTime = 0.0f;
        currentScale = startScale;
        currentAlpha = startAlpha;
        
        DEBUG_LOG("Effect started: " + effectName);
    }

    void EffectComponent::Pause()
    {
        if (state == EffectState::Playing)
        {
            state = EffectState::Paused;
        }
    }

    void EffectComponent::Resume()
    {
        if (state == EffectState::Paused)
        {
            state = EffectState::Playing;
        }
    }

    void EffectComponent::Stop()
    {
        state = EffectState::Stopped;
        currentTime = 0.0f;
        currentScale = startScale;
        currentAlpha = startAlpha;
    }

    void EffectComponent::Restart()
    {
        Stop();
        Play();
    }

    void EffectComponent::SetAnimationClip(std::shared_ptr<AnimationClip> clip)
    {
        animationClip = clip;
        if (animationClip)
        {
            duration = animationClip->GetLength();
        }
    }

    void EffectComponent::SetDuration(float dur)
    {
        duration = std::max(0.1f, dur);
    }

    void EffectComponent::SetPlaybackSpeed(float speed)
    {
        playbackSpeed = std::max(0.1f, speed);
    }

    void EffectComponent::SetScaleOverTime(bool enable, const Vector2& start, const Vector2& end)
    {
        scaleOverTime = enable;
        startScale = start;
        endScale = end;
        currentScale = startScale;
    }

    void EffectComponent::SetAlphaOverTime(bool enable, float start, float end)
    {
        alphaOverTime = enable;
        startAlpha = std::max(0.0f, std::min(1.0f, start));
        endAlpha = std::max(0.0f, std::min(1.0f, end));
        currentAlpha = startAlpha;
    }

    void EffectComponent::SetCurrentScale(const Vector2& scale)
    {
        currentScale = scale;
        ApplyVisualChanges();
    }

    void EffectComponent::SetCurrentAlpha(float alpha)
    {
        currentAlpha = std::max(0.0f, std::min(1.0f, alpha));
        ApplyVisualChanges();
    }

    void EffectComponent::UpdateAnimation(float deltaTime)
    {
        currentTime += deltaTime * playbackSpeed;
        
        if (currentTime >= duration)
        {
            if (loop)
            {
                currentTime = fmod(currentTime, duration);
            }
            else
            {
                currentTime = duration;
                state = EffectState::Finished;
                
                if (autoDestroy && gameObject)
                {
                    gameObject->SetActive(false);
                    DEBUG_LOG("Effect finished and auto-destroyed: " + effectName);
                }
            }
        }
    }

    void EffectComponent::UpdateScaling(float deltaTime)
    {
        if (!scaleOverTime || duration <= 0.0f) return;
        
        float t = currentTime / duration;
        t = EaseInOut(t);
        
        currentScale.x = startScale.x + (endScale.x - startScale.x) * t;
        currentScale.y = startScale.y + (endScale.y - startScale.y) * t;
    }

    void EffectComponent::UpdateAlpha(float deltaTime)
    {
        if (!alphaOverTime || duration <= 0.0f) return;
        
        float t = currentTime / duration;
        t = EaseInOut(t);
        
        currentAlpha = startAlpha + (endAlpha - startAlpha) * t;
    }

    void EffectComponent::ApplyVisualChanges()
    {
        if (!spriteRenderer) return;
        
        auto transform = GetGameObject()->GetComponent<Transform>();
        if (transform && scaleOverTime)
        {
            transform->SetLocalScale(currentScale);
        }
        
        if (alphaOverTime)
        {
            Color color = spriteRenderer->GetColor();
            color.a = currentAlpha;
            spriteRenderer->SetColor(color);
        }
    }

    float EffectComponent::EaseInOut(float t)
    {
        return t * t * (3.0f - 2.0f * t);
    }
}