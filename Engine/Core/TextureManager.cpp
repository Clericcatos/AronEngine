#include "TextureManager.h"
#include "framework.h"
#include <algorithm>

namespace AronEngine
{
    TextureManager& TextureManager::GetInstance()
    {
        static TextureManager instance;
        return instance;
    }

    bool TextureManager::Initialize(ID2D1RenderTarget* renderTarget)
    {
        this->renderTarget = renderTarget;
        
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wicFactory)
        );
        
        if (FAILED(hr)) {
            DEBUG_LOG("Failed to create WIC Imaging Factory");
            return false;
        }
        
        DEBUG_LOG("TextureManager initialized successfully");
        return true;
    }

    void TextureManager::Shutdown()
    {
        UnloadAllTextures();
        
        if (wicFactory) {
            wicFactory->Release();
            wicFactory = nullptr;
        }
        
        renderTarget = nullptr;
        DEBUG_LOG("TextureManager shutdown");
    }

    std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string& filepath)
    {
        auto it = loadedTextures.find(filepath);
        if (it != loadedTextures.end()) {
            DEBUG_LOG("Texture already loaded: " + filepath);
            return it->second;
        }

        if (!IsSupportedFormat(filepath)) {
            DEBUG_LOG("Unsupported texture format: " + filepath);
            return nullptr;
        }

        auto texture = std::make_shared<Texture>();
        texture->filepath = filepath;

        HRESULT hr = LoadBitmapFromFile(StringToWString(filepath), &texture->bitmap);
        if (FAILED(hr) || !texture->bitmap) {
            DEBUG_LOG("Failed to load texture: " + filepath);
            return nullptr;
        }

        texture->size = texture->bitmap->GetPixelSize();
        loadedTextures[filepath] = texture;
        
        DEBUG_LOG("Texture loaded successfully: " + filepath + 
                  " (" + std::to_string(texture->size.width) + "x" + std::to_string(texture->size.height) + ")");
        
        return texture;
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& filepath)
    {
        auto it = loadedTextures.find(filepath);
        return (it != loadedTextures.end()) ? it->second : nullptr;
    }

    void TextureManager::UnloadTexture(const std::string& filepath)
    {
        auto it = loadedTextures.find(filepath);
        if (it != loadedTextures.end()) {
            DEBUG_LOG("Unloading texture: " + filepath);
            loadedTextures.erase(it);
        }
    }

    void TextureManager::UnloadAllTextures()
    {
        DEBUG_LOG("Unloading all textures (" + std::to_string(loadedTextures.size()) + " textures)");
        loadedTextures.clear();
    }

    bool TextureManager::IsSupportedFormat(const std::string& filepath)
    {
        std::string ext = GetFileExtension(filepath);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        return (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || 
                ext == ".bmp" || ext == ".gif" || ext == ".tiff" || ext == ".ico");
    }

    void TextureManager::GetLoadedTextures(std::vector<std::string>& outPaths) const
    {
        outPaths.clear();
        outPaths.reserve(loadedTextures.size());
        
        for (const auto& pair : loadedTextures) {
            outPaths.push_back(pair.first);
        }
    }

    std::shared_ptr<Texture> TextureManager::CreateSolidColorTexture(const std::string& name, 
                                                                   int width, int height, 
                                                                   const Color& color)
    {
        if (!renderTarget) {
            DEBUG_LOG("TextureManager not initialized");
            return nullptr;
        }

        auto it = loadedTextures.find(name);
        if (it != loadedTextures.end()) {
            return it->second;
        }

        auto texture = std::make_shared<Texture>();
        texture->filepath = name;
        texture->size = D2D1::SizeU(width, height);

        D2D1_BITMAP_PROPERTIES bitmapProps = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f,
            96.0f
        );

        std::vector<uint32_t> pixels(width * height);
        uint32_t colorValue = 
            (static_cast<uint32_t>(color.a * 255) << 24) |
            (static_cast<uint32_t>(color.r * 255) << 16) |
            (static_cast<uint32_t>(color.g * 255) << 8) |
            (static_cast<uint32_t>(color.b * 255));

        std::fill(pixels.begin(), pixels.end(), colorValue);

        HRESULT hr = renderTarget->CreateBitmap(
            texture->size,
            pixels.data(),
            width * 4,
            &bitmapProps,
            &texture->bitmap
        );

        if (FAILED(hr)) {
            DEBUG_LOG("Failed to create solid color bitmap: " + name);
            return nullptr;
        }

        loadedTextures[name] = texture;
        
        DEBUG_LOG("Created solid color texture: " + name + 
                  " (" + std::to_string(width) + "x" + std::to_string(height) + ")");
        
        return texture;
    }

    HRESULT TextureManager::LoadBitmapFromFile(const std::wstring& filepath, ID2D1Bitmap** bitmap)
    {
        if (!renderTarget || !wicFactory) {
            return E_FAIL;
        }

        IWICBitmapDecoder* decoder = nullptr;
        IWICBitmapFrameDecode* frame = nullptr;
        IWICFormatConverter* converter = nullptr;

        HRESULT hr = wicFactory->CreateDecoderFromFilename(
            filepath.c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &decoder
        );

        if (SUCCEEDED(hr)) {
            hr = decoder->GetFrame(0, &frame);
        }

        if (SUCCEEDED(hr)) {
            hr = wicFactory->CreateFormatConverter(&converter);
        }

        if (SUCCEEDED(hr)) {
            hr = converter->Initialize(
                frame,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0f,
                WICBitmapPaletteTypeMedianCut
            );
        }

        if (SUCCEEDED(hr)) {
            hr = renderTarget->CreateBitmapFromWicBitmap(
                converter,
                nullptr,
                bitmap
            );
        }

        if (converter) converter->Release();
        if (frame) frame->Release();
        if (decoder) decoder->Release();

        return hr;
    }

    std::wstring TextureManager::StringToWString(const std::string& str)
    {
        if (str.empty()) return std::wstring();

        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], sizeNeeded);
        return wstrTo;
    }

    std::string TextureManager::GetFileExtension(const std::string& filepath)
    {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) {
            return "";
        }
        return filepath.substr(dotPos);
    }
}