#include "framework.h"
#include "AnimationClip.h"
#include "../Components/SpriteRenderer.h"

namespace AronEngine
{
    AnimationClip::AnimationClip(const std::string& clipName)
        : name(clipName)
        , length(0.0f)
        , frameRate(12.0f)
        , wrapMode(WrapMode::Loop)
        , isLooping(true)
    {
    }

    void AnimationClip::AddFrame(const AnimationFrame& frame)
    {
        frames.push_back(frame);
        RecalculateLength();
    }

    void AnimationClip::AddFrame(std::shared_ptr<Sprite> sprite, float duration)
    {
        frames.emplace_back(sprite, duration);
        RecalculateLength();
    }

    void AnimationClip::RemoveFrame(int index)
    {
        if (index >= 0 && index < static_cast<int>(frames.size()))
        {
            frames.erase(frames.begin() + index);
            RecalculateLength();
        }
    }

    void AnimationClip::ClearFrames()
    {
        frames.clear();
        length = 0.0f;
    }

    const AnimationFrame& AnimationClip::GetFrame(int index) const
    {
        static AnimationFrame emptyFrame(nullptr);
        if (index >= 0 && index < static_cast<int>(frames.size()))
        {
            return frames[index];
        }
        return emptyFrame;
    }

    void AnimationClip::SetFrameRate(float fps)
    {
        frameRate = fps > 0 ? fps : 1.0f;
        
        for (auto& frame : frames)
        {
            frame.duration = 1.0f / frameRate;
        }
        
        RecalculateLength();
    }

    AnimationFrame AnimationClip::SampleAnimation(float time) const
    {
        if (frames.empty())
        {
            return AnimationFrame(nullptr);
        }

        if (time <= 0.0f)
        {
            return frames[0];
        }

        float normalizedTime = time;
        
        if (wrapMode == WrapMode::Loop && length > 0.0f)
        {
            normalizedTime = fmod(time, length);
        }
        else if (wrapMode == WrapMode::PingPong && length > 0.0f)
        {
            float cycle = fmod(time, length * 2.0f);
            normalizedTime = cycle > length ? (length * 2.0f - cycle) : cycle;
        }
        else if (wrapMode == WrapMode::Once || wrapMode == WrapMode::ClampForever)
        {
            normalizedTime = (std::min)(time, length);
        }

        int frameIndex = GetFrameAtTime(normalizedTime);
        if (frameIndex >= 0 && frameIndex < static_cast<int>(frames.size()))
        {
            return frames[frameIndex];
        }
        
        return frames.empty() ? AnimationFrame(nullptr) : frames.back();
    }

    int AnimationClip::GetFrameAtTime(float time) const
    {
        if (frames.empty() || time <= 0.0f)
        {
            return 0;
        }

        float currentTime = 0.0f;
        for (int i = 0; i < static_cast<int>(frames.size()); ++i)
        {
            currentTime += frames[i].duration;
            if (time <= currentTime)
            {
                return i;
            }
        }

        return static_cast<int>(frames.size()) - 1;
    }

    void AnimationClip::RecalculateLength()
    {
        length = 0.0f;
        for (const auto& frame : frames)
        {
            length += frame.duration;
        }
    }

    float AnimationClip::GetFrameTime(int frameIndex) const
    {
        if (frameIndex < 0 || frameIndex >= static_cast<int>(frames.size()))
        {
            return 0.0f;
        }

        float time = 0.0f;
        for (int i = 0; i < frameIndex; ++i)
        {
            time += frames[i].duration;
        }
        
        return time;
    }
}