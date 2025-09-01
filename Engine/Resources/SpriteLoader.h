#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../Utils/Vector2.h"

namespace AronEngine
{
    class Sprite;
    class AnimationClip;

    struct SpriteInfo
    {
        std::string name;
        float x, y;
        float width, height;
        float pivotX, pivotY;

        SpriteInfo() : x(0), y(0), width(0), height(0), pivotX(0.5f), pivotY(0.5f) {}
    };

    struct FrameInfo
    {
        std::string spriteName;
        int spriteSheetIndex;
        float duration;

        FrameInfo() : spriteSheetIndex(0), duration(1.0f/12.0f) {}
    };

    struct EventInfo
    {
        std::string functionName;
        std::string parameter;
        float time;

        EventInfo() : time(0.0f) {}
    };

    struct SpriteSheet
    {
        std::string texturePath;
        float textureWidth, textureHeight;
        std::vector<SpriteInfo> sprites;
        std::unordered_map<std::string, int> spriteIndexMap;

        SpriteSheet() : textureWidth(0), textureHeight(0) {}
    };

    struct AnimationClipData
    {
        std::string clipName;
        std::string texturePath;
        bool loop;
        float duration;
        std::vector<FrameInfo> frames;
        std::vector<EventInfo> events;

        AnimationClipData() : loop(true), duration(1.0f) {}
    };

    class SpriteLoader
    {
    public:
        static bool LoadSpriteSheet(const std::string& filePath, SpriteSheet& spriteSheet);
        static bool LoadAnimationClip(const std::string& filePath, AnimationClipData& clipData, const SpriteSheet& spriteSheet);
        
        static std::shared_ptr<AnimationClip> CreateAnimationClipFromData(const AnimationClipData& clipData, const SpriteSheet& spriteSheet);
        static std::shared_ptr<Sprite> CreateSpriteFromInfo(const SpriteInfo& info, const std::string& texturePath);

    private:
        static bool ParseSpriteSheetJSON(const std::string& filePath, SpriteSheet& spriteSheet);
        static bool ParseAnimationClipJSON(const std::string& filePath, AnimationClipData& clipData);
    };
}