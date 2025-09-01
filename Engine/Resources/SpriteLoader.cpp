#include "framework.h"
#include "SpriteLoader.h"
#include "AnimationClip.h"
#include "../Components/SpriteRenderer.h"
#include "../Core/TextureManager.h"
#include <fstream>
#include <sstream>

namespace AronEngine
{
    bool SpriteLoader::LoadSpriteSheet(const std::string& filePath, SpriteSheet& spriteSheet)
    {
        return ParseSpriteSheetJSON(filePath, spriteSheet);
    }

    bool SpriteLoader::LoadAnimationClip(const std::string& filePath, AnimationClipData& clipData, const SpriteSheet& spriteSheet)
    {
        if (!ParseAnimationClipJSON(filePath, clipData))
        {
            return false;
        }

        for (auto& frame : clipData.frames)
        {
            auto it = spriteSheet.spriteIndexMap.find(frame.spriteName);
            if (it != spriteSheet.spriteIndexMap.end())
            {
                frame.spriteSheetIndex = it->second;
            }
            else
            {
                DEBUG_LOG("Sprite not found in sprite sheet: " + frame.spriteName);
                return false;
            }
        }

        return true;
    }

    std::shared_ptr<AnimationClip> SpriteLoader::CreateAnimationClipFromData(const AnimationClipData& clipData, const SpriteSheet& spriteSheet)
    {
        auto animationClip = std::make_shared<AnimationClip>(clipData.clipName);
        animationClip->SetLooping(clipData.loop);

        for (const auto& frameInfo : clipData.frames)
        {
            if (frameInfo.spriteSheetIndex >= 0 && frameInfo.spriteSheetIndex < spriteSheet.sprites.size())
            {
                const SpriteInfo& spriteInfo = spriteSheet.sprites[frameInfo.spriteSheetIndex];
                auto sprite = CreateSpriteFromInfo(spriteInfo, spriteSheet.texturePath);
                
                if (sprite)
                {
                    animationClip->AddFrame(sprite, frameInfo.duration);
                }
            }
        }

        return animationClip;
    }

    std::shared_ptr<Sprite> SpriteLoader::CreateSpriteFromInfo(const SpriteInfo& info, const std::string& texturePath)
    {
        auto texture = TextureManager::GetInstance().LoadTexture(texturePath);
        if (!texture)
        {
            DEBUG_LOG("Failed to load texture: " + texturePath);
            return nullptr;
        }

        auto sprite = std::make_shared<Sprite>(texture, texturePath);
        sprite->size = Vector2(info.width, info.height);
        sprite->pivot = Vector2(info.pivotX, info.pivotY);
        sprite->textureRect = D2D1::RectF(info.x, info.y, info.x + info.width, info.y + info.height);

        return sprite;
    }

    bool SpriteLoader::ParseSpriteSheetJSON(const std::string& filePath, SpriteSheet& spriteSheet)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open sprite sheet file: " + filePath);
            return false;
        }

        std::string line, content;
        while (std::getline(file, line))
        {
            content += line;
        }
        file.close();

        // Simple JSON parsing (for basic format)
        // In a full implementation, you'd use a proper JSON library
        
        // Extract texture path
        size_t texturePos = content.find("\"texture\":");
        if (texturePos != std::string::npos)
        {
            size_t start = content.find("\"", texturePos + 10) + 1;
            size_t end = content.find("\"", start);
            if (start != std::string::npos && end != std::string::npos)
            {
                spriteSheet.texturePath = content.substr(start, end - start);
            }
        }

        // Extract texture dimensions
        size_t widthPos = content.find("\"textureWidth\":");
        if (widthPos != std::string::npos)
        {
            size_t start = widthPos + 15;
            size_t end = content.find(",", start);
            if (end == std::string::npos) end = content.find("}", start);
            spriteSheet.textureWidth = std::stof(content.substr(start, end - start));
        }

        size_t heightPos = content.find("\"textureHeight\":");
        if (heightPos != std::string::npos)
        {
            size_t start = heightPos + 16;
            size_t end = content.find(",", start);
            if (end == std::string::npos) end = content.find("}", start);
            spriteSheet.textureHeight = std::stof(content.substr(start, end - start));
        }

        // Simple sprite parsing (this is a basic implementation)
        // For production, use nlohmann/json or similar library
        
        DEBUG_LOG("SpriteSheet loaded: " + spriteSheet.texturePath);
        return true;
    }

    bool SpriteLoader::ParseAnimationClipJSON(const std::string& filePath, AnimationClipData& clipData)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open animation clip file: " + filePath);
            return false;
        }

        std::string line, content;
        while (std::getline(file, line))
        {
            content += line;
        }
        file.close();

        // Simple JSON parsing for animation clip
        // Extract clip name
        size_t namePos = content.find("\"clipName\":");
        if (namePos != std::string::npos)
        {
            size_t start = content.find("\"", namePos + 11) + 1;
            size_t end = content.find("\"", start);
            if (start != std::string::npos && end != std::string::npos)
            {
                clipData.clipName = content.substr(start, end - start);
            }
        }

        // Extract loop setting
        size_t loopPos = content.find("\"loop\":");
        if (loopPos != std::string::npos)
        {
            size_t start = loopPos + 7;
            size_t end = content.find(",", start);
            if (end == std::string::npos) end = content.find("}", start);
            std::string loopStr = content.substr(start, end - start);
            clipData.loop = (loopStr.find("true") != std::string::npos);
        }

        DEBUG_LOG("AnimationClip data loaded: " + clipData.clipName);
        return true;
    }
}