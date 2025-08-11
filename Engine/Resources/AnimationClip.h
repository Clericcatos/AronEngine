#pragma once
#include "../Core/framework.h"
#include "../Utils/Vector2.h"
#include "../Components/SpriteRenderer.h"
#include <vector>
#include <memory>

namespace AronEngine
{
    class Texture2D;

    struct AnimationFrame
    {
        std::shared_ptr<Sprite> sprite;
        float duration;
        Vector2 pivot;
        
        AnimationFrame(std::shared_ptr<Sprite> s, float d = 1.0f/12.0f) 
            : sprite(s), duration(d), pivot(0.5f, 0.5f) {}
    };

    enum class WrapMode
    {
        Default = 0,
        Once = 1,
        Loop = 2,
        PingPong = 4,
        ClampForever = 8
    };

    class AnimationClip
    {
    private:
        std::string name;
        std::vector<AnimationFrame> frames;
        float length;
        float frameRate;
        WrapMode wrapMode;
        bool isLooping;

    public:
        AnimationClip(const std::string& clipName = "New Animation");
        ~AnimationClip() = default;

        void AddFrame(const AnimationFrame& frame);
        void AddFrame(std::shared_ptr<Sprite> sprite, float duration = 1.0f/12.0f);
        void RemoveFrame(int index);
        void ClearFrames();

        const AnimationFrame& GetFrame(int index) const;
        int GetFrameCount() const { return static_cast<int>(frames.size()); }
        
        void SetName(const std::string& newName) { name = newName; }
        const std::string& GetName() const { return name; }
        
        void SetFrameRate(float fps);
        float GetFrameRate() const { return frameRate; }
        
        void SetWrapMode(WrapMode mode) { wrapMode = mode; }
        WrapMode GetWrapMode() const { return wrapMode; }
        
        void SetLooping(bool loop) { isLooping = loop; }
        bool IsLooping() const { return isLooping; }
        
        float GetLength() const { return length; }
        
        AnimationFrame SampleAnimation(float time) const;
        int GetFrameAtTime(float time) const;
        
    private:
        void RecalculateLength();
        float GetFrameTime(int frameIndex) const;
    };
}