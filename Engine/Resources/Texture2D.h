#pragma once
#include "../Core/framework.h"
#include "../Utils/Vector2.h"

namespace AronEngine
{
    class Texture2D
    {
    private:
        ComPtr<ID2D1Bitmap1> bitmap;
        Vector2 size;
        std::string filePath;

    public:
        Texture2D();
        ~Texture2D() = default;

        bool LoadFromFile(const std::string& filePath, ID2D1DeviceContext6* context, IWICImagingFactory2* wicFactory);
        bool CreateFromMemory(const void* data, size_t dataSize, ID2D1DeviceContext6* context, IWICImagingFactory2* wicFactory);

        ID2D1Bitmap1* GetBitmap() const { return bitmap.Get(); }
        const Vector2& GetSize() const { return size; }
        float GetWidth() const { return size.x; }
        float GetHeight() const { return size.y; }
        const std::string& GetFilePath() const { return filePath; }

    private:
        bool LoadFromWICBitmap(IWICBitmapSource* wicBitmap, ID2D1DeviceContext6* context);
    };
}