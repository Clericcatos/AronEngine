#include "framework.h"
#include "ResourceManager.h"
#include "../Resources/Texture2D.h"
#include <filesystem>

namespace AronEngine
{
    ResourceManager::ResourceManager()
    {
        resourcePath = "./Resources/";
    }

    std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& filePath)
    {
        auto it = textures.find(filePath);
        if (it != textures.end())
        {
            return it->second;
        }

        auto texture = std::make_shared<Texture2D>();
        std::string fullPath = GetFullPath(filePath);
        
        textures[filePath] = texture;
        return texture;
    }

    std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string& name)
    {
        auto it = textures.find(name);
        if (it != textures.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void ResourceManager::UnloadTexture(const std::string& name)
    {
        textures.erase(name);
    }

    void ResourceManager::UnloadAllTextures()
    {
        textures.clear();
    }

    std::string ResourceManager::GetFullPath(const std::string& relativePath) const
    {
        return resourcePath + relativePath;
    }
}