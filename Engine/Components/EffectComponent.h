#pragma once
#include "Component.h"
#include "../Utils/Vector2.h"
#include <string>
#include <memory>

namespace AronEngine
{
    class AnimationClip;
    class SpriteRenderer;

    enum class EffectState
    {
        Stopped,
        Playing,
        Paused,
        Finished
    };

    class EffectComponent : public Component
    {
    private:
        std::shared_ptr<AnimationClip> animationClip;
        SpriteRenderer* spriteRenderer;
        
        std::string effectName;
        EffectState state;
        
        float duration;
        float currentTime;
        float playbackSpeed;
        
        Vector2 startScale;
        Vector2 endScale;
        Vector2 currentScale;
        
        float startAlpha;
        float endAlpha;
        float currentAlpha;
        
        bool loop;
        bool autoDestroy;
        bool scaleOverTime;
        bool alphaOverTime;
        
        float scaleEaseType;
        float alphaEaseType;

    public:
        EffectComponent();
        virtual ~EffectComponent() override = default;

        void Start() override;
        void Update(float deltaTime) override;
        void OnDestroy() override;

        void Play();
        void Pause();
        void Resume();
        void Stop();
        void Restart();

        void SetEffectName(const std::string& name) { effectName = name; }
        const std::string& GetEffectName() const { return effectName; }

        void SetAnimationClip(std::shared_ptr<AnimationClip> clip);
        std::shared_ptr<AnimationClip> GetAnimationClip() const { return animationClip; }

        void SetDuration(float dur);
        float GetDuration() const { return duration; }

        void SetPlaybackSpeed(float speed);
        float GetPlaybackSpeed() const { return playbackSpeed; }

        void SetLoop(bool shouldLoop) { loop = shouldLoop; }
        bool GetLoop() const { return loop; }

        void SetAutoDestroy(bool shouldAutoDestroy) { autoDestroy = shouldAutoDestroy; }
        bool GetAutoDestroy() const { return autoDestroy; }

        void SetScaleOverTime(bool enable, const Vector2& start, const Vector2& end);
        void SetAlphaOverTime(bool enable, float start, float end);

        void SetCurrentScale(const Vector2& scale);
        Vector2 GetCurrentScale() const { return currentScale; }

        void SetCurrentAlpha(float alpha);
        float GetCurrentAlpha() const { return currentAlpha; }

        EffectState GetState() const { return state; }
        float GetNormalizedTime() const { return duration > 0.0f ? currentTime / duration : 0.0f; }

        bool IsPlaying() const { return state == EffectState::Playing; }
        bool IsFinished() const { return state == EffectState::Finished; }

    private:
        void UpdateAnimation(float deltaTime);
        void UpdateScaling(float deltaTime);
        void UpdateAlpha(float deltaTime);
        void ApplyVisualChanges();
        float EaseInOut(float t);
    };
}