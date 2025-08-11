#include "framework.h"
#include "Texture2D.h"

namespace AronEngine
{
    Texture2D::Texture2D()
        : size(0, 0)
    {
    }

    bool Texture2D::LoadFromFile(const std::string& filePath, ID2D1DeviceContext6* context, IWICImagingFactory2* wicFactory)
    {
        this->filePath = filePath;
        HRESULT hr = S_OK;

        ComPtr<IWICBitmapDecoder> decoder;
        std::wstring wFilePath(filePath.begin(), filePath.end());
        
        hr = wicFactory->CreateDecoderFromFilename(
            wFilePath.c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            decoder.GetAddressOf()
        );

        if (FAILED(hr))
            return false;

        ComPtr<IWICBitmapFrameDecode> source;
        hr = decoder->GetFrame(0, source.GetAddressOf());
        if (FAILED(hr))
            return false;

        ComPtr<IWICFormatConverter> converter;
        hr = wicFactory->CreateFormatConverter(converter.GetAddressOf());
        if (FAILED(hr))
            return false;

        hr = converter->Initialize(
            source.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeMedianCut
        );

        if (FAILED(hr))
            return false;

        return LoadFromWICBitmap(converter.Get(), context);
    }

    bool Texture2D::CreateFromMemory(const void* data, size_t dataSize, ID2D1DeviceContext6* context, IWICImagingFactory2* wicFactory)
    {
        HRESULT hr = S_OK;

        ComPtr<IWICStream> stream;
        hr = wicFactory->CreateStream(stream.GetAddressOf());
        if (FAILED(hr))
            return false;

        hr = stream->InitializeFromMemory(static_cast<BYTE*>(const_cast<void*>(data)), static_cast<DWORD>(dataSize));
        if (FAILED(hr))
            return false;

        ComPtr<IWICBitmapDecoder> decoder;
        hr = wicFactory->CreateDecoderFromStream(
            stream.Get(),
            nullptr,
            WICDecodeMetadataCacheOnLoad,
            decoder.GetAddressOf()
        );

        if (FAILED(hr))
            return false;

        ComPtr<IWICBitmapFrameDecode> source;
        hr = decoder->GetFrame(0, source.GetAddressOf());
        if (FAILED(hr))
            return false;

        ComPtr<IWICFormatConverter> converter;
        hr = wicFactory->CreateFormatConverter(converter.GetAddressOf());
        if (FAILED(hr))
            return false;

        hr = converter->Initialize(
            source.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeMedianCut
        );

        if (FAILED(hr))
            return false;

        return LoadFromWICBitmap(converter.Get(), context);
    }

    bool Texture2D::LoadFromWICBitmap(IWICBitmapSource* wicBitmap, ID2D1DeviceContext6* context)
    {
        HRESULT hr = context->CreateBitmapFromWicBitmap(wicBitmap, nullptr, bitmap.GetAddressOf());
        if (FAILED(hr))
            return false;

        D2D1_SIZE_F d2dSize = bitmap->GetSize();
        size = Vector2(d2dSize.width, d2dSize.height);

        return true;
    }
}