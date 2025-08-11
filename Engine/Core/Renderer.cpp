#include "framework.h"
#include "Renderer.h"
#include "../Resources/Texture2D.h"

namespace AronEngine
{
    Renderer::Renderer()
        : hWnd(nullptr)
        , width(0)
        , height(0)
    {
    }

    Renderer::~Renderer()
    {
        Shutdown();
    }

    bool Renderer::Initialize(HWND hWnd, int width, int height)
    {
        this->hWnd = hWnd;
        this->width = width;
        this->height = height;

        HRESULT hr = S_OK;

        // Create D2D factory
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf());
        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create D2D factory");
            return false;
        }

        // Create DWrite factory
        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7),
            reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()));
        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create DWrite factory");
            return false;
        }

        // Create WIC factory
        hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(wicFactory.GetAddressOf()));
        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create WIC factory");
            return false;
        }

        if (!CreateDeviceResources())
        {
            return false;
        }

        if (!CreateRenderTarget())
        {
            return false;
        }

        return true;
    }

    void Renderer::Shutdown()
    {
        ReleaseRenderTarget();
        targetBitmap.Reset();
        swapChain.Reset();
        dxgiBackBuffer.Reset();
        d3dContext.Reset();
        d3dDevice.Reset();
        d2dContext.Reset();
        d2dDevice.Reset();
        wicFactory.Reset();
        dwriteFactory.Reset();
        d2dFactory.Reset();
    }

    void Renderer::Resize(int width, int height)
    {
        this->width = width;
        this->height = height;

        if (swapChain)
        {
            d2dContext->SetTarget(nullptr);
            targetBitmap.Reset();
            dxgiBackBuffer.Reset();

            HRESULT hr = swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
            if (SUCCEEDED(hr))
            {
                CreateRenderTarget();
            }
        }
    }

    bool Renderer::CreateDeviceResources()
    {
        HRESULT hr = S_OK;

        // Create D3D device
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };

        D3D_FEATURE_LEVEL featureLevel;
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
            featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
            d3dDevice.GetAddressOf(), &featureLevel, d3dContext.GetAddressOf());

        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create D3D device");
            return false;
        }

        // Get DXGI device
        ComPtr<IDXGIDevice> dxgiDevice;
        hr = d3dDevice.As(&dxgiDevice);
        if (FAILED(hr))
        {
            return false;
        }

        // Create D2D device
        hr = d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf());
        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create D2D device");
            return false;
        }

        // Create D2D device context
        hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dContext.GetAddressOf());
        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create D2D device context");
            return false;
        }

        // Create swap chain
        ComPtr<IDXGIAdapter> dxgiAdapter;
        ComPtr<IDXGIFactory2> dxgiFactory;
        
        hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            hr = dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
        }

        if (SUCCEEDED(hr))
        {
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = width;
            swapChainDesc.Height = height;
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

            hr = dxgiFactory->CreateSwapChainForHwnd(d3dDevice.Get(), hWnd, &swapChainDesc,
                nullptr, nullptr, swapChain.GetAddressOf());
        }

        if (FAILED(hr))
        {
            DEBUG_LOG("Failed to create swap chain");
            return false;
        }

        return true;
    }

    bool Renderer::CreateRenderTarget()
    {
        HRESULT hr = S_OK;

        // Get back buffer
        hr = swapChain->GetBuffer(0, IID_PPV_ARGS(dxgiBackBuffer.GetAddressOf()));
        if (FAILED(hr))
        {
            return false;
        }

        // Create bitmap from DXGI surface
        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));

        hr = d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties,
            targetBitmap.GetAddressOf());
        if (FAILED(hr))
        {
            return false;
        }

        d2dContext->SetTarget(targetBitmap.Get());
        d2dContext->SetDpi(96.0f, 96.0f);

        return true;
    }

    void Renderer::ReleaseRenderTarget()
    {
        d2dContext->SetTarget(nullptr);
        targetBitmap.Reset();
        dxgiBackBuffer.Reset();
    }

    void Renderer::BeginDraw()
    {
        d2dContext->BeginDraw();
    }

    void Renderer::EndDraw()
    {
        HRESULT hr = d2dContext->EndDraw();
        if (SUCCEEDED(hr))
        {
            swapChain->Present(1, 0);
        }
    }

    void Renderer::Clear(const Color& color)
    {
        d2dContext->Clear(D2D1::ColorF(color.r, color.g, color.b, color.a));
    }

    void Renderer::SetTransform(const Matrix3x2& transform)
    {
        d2dContext->SetTransform(D2D1::Matrix3x2F(
            transform.m11, transform.m12,
            transform.m21, transform.m22,
            transform.m31, transform.m32
        ));
    }

    Matrix3x2 Renderer::GetTransform() const
    {
        D2D1_MATRIX_3X2_F d2dMatrix;
        d2dContext->GetTransform(&d2dMatrix);
        return Matrix3x2(
            d2dMatrix._11, d2dMatrix._12,
            d2dMatrix._21, d2dMatrix._22,
            d2dMatrix._31, d2dMatrix._32
        );
    }

    void Renderer::PushTransform(const Matrix3x2& transform)
    {
        auto currentTransform = GetTransform();
        SetTransform(currentTransform * transform);
    }

    void Renderer::PopTransform()
    {
        SetTransform(Matrix3x2::Identity());
    }

    void Renderer::DrawLine(const Vector2& start, const Vector2& end, const Color& color, float strokeWidth)
    {
        auto brush = CreateSolidBrush(color);
        d2dContext->DrawLine(
            D2D1::Point2F(start.x, start.y),
            D2D1::Point2F(end.x, end.y),
            brush.Get(),
            strokeWidth
        );
    }

    void Renderer::DrawRectangle(const Vector2& position, const Vector2& size, const Color& color, float strokeWidth)
    {
        auto brush = CreateSolidBrush(color);
        D2D1_RECT_F rect = D2D1::RectF(position.x, position.y, position.x + size.x, position.y + size.y);
        d2dContext->DrawRectangle(rect, brush.Get(), strokeWidth);
    }

    void Renderer::FillRectangle(const Vector2& position, const Vector2& size, const Color& color)
    {
        auto brush = CreateSolidBrush(color);
        D2D1_RECT_F rect = D2D1::RectF(position.x, position.y, position.x + size.x, position.y + size.y);
        d2dContext->FillRectangle(rect, brush.Get());
    }

    void Renderer::DrawCircle(const Vector2& center, float radius, const Color& color, float strokeWidth)
    {
        auto brush = CreateSolidBrush(color);
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius);
        d2dContext->DrawEllipse(ellipse, brush.Get(), strokeWidth);
    }

    void Renderer::FillCircle(const Vector2& center, float radius, const Color& color)
    {
        auto brush = CreateSolidBrush(color);
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius);
        d2dContext->FillEllipse(ellipse, brush.Get());
    }

    void Renderer::DrawEllipse(const Vector2& center, const Vector2& radius, const Color& color, float strokeWidth)
    {
        auto brush = CreateSolidBrush(color);
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius.x, radius.y);
        d2dContext->DrawEllipse(ellipse, brush.Get(), strokeWidth);
    }

    void Renderer::FillEllipse(const Vector2& center, const Vector2& radius, const Color& color)
    {
        auto brush = CreateSolidBrush(color);
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius.x, radius.y);
        d2dContext->FillEllipse(ellipse, brush.Get());
    }

    void Renderer::DrawTexture(Texture2D* texture, const Vector2& position, const Vector2& size,
                              const Color& tint, float opacity)
    {
        if (!texture || !texture->GetBitmap())
            return;

        D2D1_RECT_F destRect = D2D1::RectF(position.x, position.y, position.x + size.x, position.y + size.y);
        
        d2dContext->DrawBitmap(
            texture->GetBitmap(),
            destRect,
            opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        );
    }

    void Renderer::DrawTextureRect(Texture2D* texture, const Vector2& position, const Vector2& size,
                                  const Vector2& sourcePos, const Vector2& sourceSize,
                                  const Color& tint, float opacity)
    {
        if (!texture || !texture->GetBitmap())
            return;

        D2D1_RECT_F destRect = D2D1::RectF(position.x, position.y, position.x + size.x, position.y + size.y);
        D2D1_RECT_F srcRect = D2D1::RectF(sourcePos.x, sourcePos.y, sourcePos.x + sourceSize.x, sourcePos.y + sourceSize.y);
        
        d2dContext->DrawBitmap(
            texture->GetBitmap(),
            destRect,
            opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            &srcRect
        );
    }

    void Renderer::DrawText(const std::wstring& text, const Vector2& position, const Color& color,
                           const std::wstring& fontName, float fontSize)
    {
        auto brush = CreateSolidBrush(color);
        auto textFormat = CreateTextFormat(fontName, fontSize);
        
        if (textFormat)
        {
            D2D1_RECT_F layoutRect = D2D1::RectF(position.x, position.y, position.x + 1000, position.y + 1000);
            d2dContext->DrawText(
                text.c_str(),
                static_cast<UINT32>(text.length()),
                textFormat.Get(),
                layoutRect,
                brush.Get()
            );
        }
    }

    ComPtr<ID2D1SolidColorBrush> Renderer::CreateSolidBrush(const Color& color)
    {
        ComPtr<ID2D1SolidColorBrush> brush;
        d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(color.r, color.g, color.b, color.a),
            brush.GetAddressOf()
        );
        return brush;
    }

    ComPtr<IDWriteTextFormat> Renderer::CreateTextFormat(const std::wstring& fontName, float fontSize)
    {
        ComPtr<IDWriteTextFormat> textFormat;
        dwriteFactory->CreateTextFormat(
            fontName.c_str(),
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            L"en-us",
            textFormat.GetAddressOf()
        );
        return textFormat;
    }
    
    void Renderer::SetTransform(const D2D1_MATRIX_3X2_F& transform)
    {
        if (d2dContext)
        {
            d2dContext->SetTransform(transform);
        }
    }
    
    void Renderer::DrawSprite(Texture2D* texture, const D2D1_RECT_F& destRect, const D2D1_RECT_F& sourceRect, const Color& tint, float opacity)
    {
        if (!d2dContext || !texture)
            return;
            
        ID2D1Bitmap* bitmap = texture->GetBitmap();
        if (!bitmap)
            return;
            
        d2dContext->DrawBitmap(
            bitmap,
            &destRect,
            opacity * tint.a,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            &sourceRect
        );
    }
    
    void Renderer::DrawTexture(std::shared_ptr<Texture> texture, const Vector2& position, const Vector2& size, const Color& tint, float opacity)
    {
        if (!d2dContext || !texture || !texture->bitmap)
            return;

        D2D1_RECT_F destRect = D2D1::RectF(
            position.x,
            position.y,
            position.x + size.x,
            position.y + size.y
        );

        D2D1_RECT_F sourceRect = D2D1::RectF(
            0.0f,
            0.0f,
            static_cast<float>(texture->size.width),
            static_cast<float>(texture->size.height)
        );

        // 색상 팅트를 적용하고 싶다면 여기서 처리
        d2dContext->DrawBitmap(
            texture->bitmap,
            &destRect,
            opacity * tint.a,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            &sourceRect
        );
    }

    void Renderer::DrawTextureRect(std::shared_ptr<Texture> texture, const Vector2& position, const Vector2& size, 
                                 const D2D1_RECT_F& sourceRect, const Color& tint, float opacity)
    {
        if (!d2dContext || !texture || !texture->bitmap)
            return;

        D2D1_RECT_F destRect = D2D1::RectF(
            position.x,
            position.y,
            position.x + size.x,
            position.y + size.y
        );

        d2dContext->DrawBitmap(
            texture->bitmap,
            &destRect,
            opacity * tint.a,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            &sourceRect
        );
    }
}