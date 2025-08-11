#pragma once
#include "../Core/framework.h"
#include "../Core/Singleton.h"
#include <unordered_map>
#include <memory>

namespace AronEngine
{
    class Texture2D;

    class ResourceManager
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
        std::string resourcePath;

    public:
        ResourceManager();
        ~ResourceManager() = default;

        void SetResourcePath(const std::string& path) { resourcePath = path; }
        const std::string& GetResourcePath() const { return resourcePath; }

        std::shared_ptr<Texture2D> LoadTexture(const std::string& filePath);
        std::shared_ptr<Texture2D> GetTexture(const std::string& name);
        
        void UnloadTexture(const std::string& name);
        void UnloadAllTextures();

    private:
        std::string GetFullPath(const std::string& relativePath) const;
    };
}