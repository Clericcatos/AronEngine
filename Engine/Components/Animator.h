#pragma once
#include "Component.h"
#include "../Resources/AnimationClip.h"
#include <memory>
#include <unordered_map>

namespace AronEngine
{
    class SpriteRenderer;

    enum class AnimatorState
    {
        Playing,
        Paused,
        Stopped
    };

    class Animator : public Component
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<AnimationClip>> animations;
        std::shared_ptr<AnimationClip> currentAnimation;
        std::string currentAnimationName;
        
        SpriteRenderer* spriteRenderer;
        
        float currentTime;
        float speed;
        AnimatorState state;
        
        bool autoPlay;
        std::string defaultAnimation;
        
        int currentFrameIndex;
        float frameTimer;

    public:
        Animator();
        virtual ~Animator() = default;

        virtual void Start() override;
        virtual void Update(float deltaTime) override;
        virtual void OnDestroy() override;

        void AddAnimation(const std::string& name, std::shared_ptr<AnimationClip> clip);
        void RemoveAnimation(const std::string& name);
        bool HasAnimation(const std::string& name) const;
        std::shared_ptr<AnimationClip> GetAnimation(const std::string& name) const;
        
        void Play(const std::string& animationName);
        void Play(); // Play current or default animation
        void Pause();
        void Stop();
        void Resume();
        
        bool IsPlaying() const { return state == AnimatorState::Playing; }
        bool IsPaused() const { return state == AnimatorState::Paused; }
        bool IsStopped() const { return state == AnimatorState::Stopped; }
        
        void SetSpeed(float newSpeed) { speed = (std::max)(0.0f, newSpeed); }
        float GetSpeed() const { return speed; }
        
        void SetAutoPlay(bool enable) { autoPlay = enable; }
        bool GetAutoPlay() const { return autoPlay; }
        
        void SetDefaultAnimation(const std::string& animationName) { defaultAnimation = animationName; }
        const std::string& GetDefaultAnimation() const { return defaultAnimation; }
        
        const std::string& GetCurrentAnimationName() const { return currentAnimationName; }
        std::shared_ptr<AnimationClip> GetCurrentAnimation() const { return currentAnimation; }
        
        float GetCurrentTime() const { return currentTime; }
        void SetCurrentTime(float time) { currentTime = (std::max)(0.0f, time); }
        
        int GetCurrentFrameIndex() const { return currentFrameIndex; }
        
        void SetSpriteRenderer(SpriteRenderer* renderer) { spriteRenderer = renderer; }
        
    private:
        void UpdateAnimation(float deltaTime);
        void UpdateSpriteRenderer();
        SpriteRenderer* FindSpriteRenderer();
    };
}