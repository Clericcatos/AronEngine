#pragma once

#include "framework.h"
#include <wincodec.h>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace AronEngine
{
    struct Texture
    {
        ID2D1Bitmap* bitmap = nullptr;
        D2D1_SIZE_U size = { 0, 0 };
        std::string filepath = "";
        
        ~Texture()
        {
            SafeRelease(&bitmap);
        }
    };

    class TextureManager
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
        ID2D1RenderTarget* renderTarget = nullptr;
        IWICImagingFactory* wicFactory = nullptr;

        TextureManager() = default;
        ~TextureManager() = default;
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        HRESULT LoadBitmapFromFile(const std::wstring& filepath, ID2D1Bitmap** bitmap);
        std::wstring StringToWString(const std::string& str);
        std::string GetFileExtension(const std::string& filepath);

    public:
        static TextureManager& GetInstance();
        
        bool Initialize(ID2D1RenderTarget* renderTarget);
        void Shutdown();
        
        std::shared_ptr<Texture> LoadTexture(const std::string& filepath);
        std::shared_ptr<Texture> GetTexture(const std::string& filepath);
        std::shared_ptr<Texture> CreateSolidColorTexture(const std::string& name, 
                                                        int width, int height, 
                                                        const Color& color);
        
        void UnloadTexture(const std::string& filepath);
        void UnloadAllTextures();
        void GetLoadedTextures(std::vector<std::string>& outPaths) const;
        bool IsSupportedFormat(const std::string& filepath);
    };
}