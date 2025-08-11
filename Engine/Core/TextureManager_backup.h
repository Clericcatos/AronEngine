#pragma once

#include "framework.h"
#include <wincodec.h>
#include <unordered_map>
#include <memory>
#include <string>

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
    public:
        static TextureManager& GetInstance();
        
        bool Initialize(ID2D1RenderTarget* renderTarget);
        void Shutdown();
        
        // 파일에서 텍스처 로드
        std::shared_ptr<Texture> LoadTexture(const std::string& filepath);
        
        // 캐시된 텍스처 가져오기
        std::shared_ptr<Texture> GetTexture(const std::string& filepath);
        
        // 단색 텍스처 생성
        std::shared_ptr<Texture> CreateSolidColorTexture(const std::string& name, 
                                                        int width, int height, 
                                                        const Color& color);
        
        // 텍스처 언로드
        void UnloadTexture(const std::string& filepath);
        void UnloadAllTextures();
        
        // 로드된 텍스처 목록
        void GetLoadedTextures(std::vector<std::string>& outPaths) const;
        
        // 지원 포맷 확인
        bool IsSupportedFormat(const std::string& filepath);
        
    private:
        TextureManager() = default;
        ~TextureManager() = default;
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        
        // 헬퍼 함수들
        HRESULT LoadBitmapFromFile(const std::wstring& filepath, ID2D1Bitmap** bitmap);
        std::wstring StringToWString(const std::string& str);
        std::string GetFileExtension(const std::string& filepath);
        
        // 멤버 변수
        std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
        ID2D1RenderTarget* renderTarget = nullptr;
        IWICImagingFactory* wicFactory = nullptr;
    };
}