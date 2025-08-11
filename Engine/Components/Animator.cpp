#include "framework.h"
#include "Animator.h"
#include "SpriteRenderer.h"
#include "../Core/GameObject.h"

namespace AronEngine
{
    Animator::Animator()
        : Component("Animator")
        , currentAnimation(nullptr)
        , spriteRenderer(nullptr)
        , currentTime(0.0f)
        , speed(1.0f)
        , state(AnimatorState::Stopped)
        , autoPlay(true)
        , currentFrameIndex(0)
        , frameTimer(0.0f)
    {
    }

    void Animator::Start()
    {
        Component::Start();
        
        spriteRenderer = FindSpriteRenderer();
        
        if (autoPlay && !defaultAnimation.empty())
        {
            Play(defaultAnimation);
        }
        else if (autoPlay && !animations.empty())
        {
            Play(animations.begin()->first);
        }
    }

    void Animator::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        if (state == AnimatorState::Playing)
        {
            UpdateAnimation(deltaTime);
        }
    }

    void Animator::OnDestroy()
    {
        animations.clear();
        currentAnimation = nullptr;
        spriteRenderer = nullptr;
        Component::OnDestroy();
    }

    void Animator::AddAnimation(const std::string& name, std::shared_ptr<AnimationClip> clip)
    {
        if (clip)
        {
            animations[name] = clip;
            
            if (defaultAnimation.empty())
            {
                defaultAnimation = name;
            }
        }
    }

    void Animator::RemoveAnimation(const std::string& name)
    {
        auto it = animations.find(name);
        if (it != animations.end())
        {
            if (currentAnimationName == name)
            {
                Stop();
                currentAnimation = nullptr;
                currentAnimationName.clear();
            }
            
            animations.erase(it);
            
            if (defaultAnimation == name)
            {
                defaultAnimation = animations.empty() ? "" : animations.begin()->first;
            }
        }
    }

    bool Animator::HasAnimation(const std::string& name) const
    {
        return animations.find(name) != animations.end();
    }

    std::shared_ptr<AnimationClip> Animator::GetAnimation(const std::string& name) const
    {
        auto it = animations.find(name);
        return (it != animations.end()) ? it->second : nullptr;
    }

    void Animator::Play(const std::string& animationName)
    {
        auto it = animations.find(animationName);
        if (it != animations.end())
        {
            currentAnimation = it->second;
            currentAnimationName = animationName;
            currentTime = 0.0f;
            currentFrameIndex = 0;
            frameTimer = 0.0f;
            state = AnimatorState::Playing;
            
            UpdateSpriteRenderer();
        }
    }

    void Animator::Play()
    {
        if (!currentAnimationName.empty())
        {
            state = AnimatorState::Playing;
        }
        else if (!defaultAnimation.empty())
        {
            Play(defaultAnimation);
        }
        else if (!animations.empty())
        {
            Play(animations.begin()->first);
        }
    }

    void Animator::Pause()
    {
        if (state == AnimatorState::Playing)
        {
            state = AnimatorState::Paused;
        }
    }

    void Animator::Stop()
    {
        state = AnimatorState::Stopped;
        currentTime = 0.0f;
        currentFrameIndex = 0;
        frameTimer = 0.0f;
        
        UpdateSpriteRenderer();
    }

    void Animator::Resume()
    {
        if (state == AnimatorState::Paused)
        {
            state = AnimatorState::Playing;
        }
    }

    void Animator::UpdateAnimation(float deltaTime)
    {
        if (!currentAnimation)
            return;

        currentTime += deltaTime * speed;
        
        AnimationFrame frame = currentAnimation->SampleAnimation(currentTime);
        currentFrameIndex = currentAnimation->GetFrameAtTime(currentTime);
        
        if (spriteRenderer && frame.sprite)
        {
            spriteRenderer->SetSprite(frame.sprite);
        }
        
        if (currentAnimation->GetWrapMode() == WrapMode::Once && 
            currentTime >= currentAnimation->GetLength())
        {
            Stop();
        }
    }

    void Animator::UpdateSpriteRenderer()
    {
        if (!spriteRenderer || !currentAnimation)
            return;
            
        AnimationFrame frame = currentAnimation->SampleAnimation(currentTime);
        if (frame.sprite)
        {
            spriteRenderer->SetSprite(frame.sprite);
        }
    }

    SpriteRenderer* Animator::FindSpriteRenderer()
    {
        if (gameObject)
        {
            return gameObject->GetComponent<SpriteRenderer>();
        }
        return nullptr;
    }
}