#pragma once
#include "framework.h"
#include "../Utils/Vector2.h"
#include "../Utils/Color.h"
#include "../Utils/Matrix3x2.h"
#include "TextureManager.h"
#include <DirectXMath.h>

namespace AronEngine
{
    class Texture2D;

    class Renderer
    {
    private:
        ComPtr<ID2D1Factory7> d2dFactory;
        ComPtr<ID2D1Device6> d2dDevice;
        ComPtr<ID2D1DeviceContext6> d2dContext;
        ComPtr<IDXGISwapChain1> swapChain;
        ComPtr<ID2D1Bitmap1> targetBitmap;
        
        ComPtr<IDWriteFactory7> dwriteFactory;
        ComPtr<IWICImagingFactory2> wicFactory;
        
        ComPtr<ID3D11Device> d3dDevice;
        ComPtr<ID3D11DeviceContext> d3dContext;
        ComPtr<IDXGISurface> dxgiBackBuffer;
        
        // D3D11 triangle rendering resources
        ComPtr<ID3D11Buffer> triangleVertexBuffer;
        ComPtr<ID3D11VertexShader> triangleVertexShader;
        ComPtr<ID3D11PixelShader> trianglePixelShader;
        ComPtr<ID3D11InputLayout> triangleInputLayout;
        ComPtr<ID3D11RenderTargetView> d3dRenderTargetView;
        UINT triangleVertexStride;
        UINT triangleVertexOffset;

        HWND hWnd;
        int width;
        int height;

    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hWnd, int width, int height);
        void Shutdown();
        void Resize(int width, int height);

        void BeginDraw();
        void EndDraw();
        void Clear(const Color& color);

        void SetTransform(const Matrix3x2& transform);
        void SetTransform(const D2D1_MATRIX_3X2_F& transform);
        Matrix3x2 GetTransform() const;
        void PushTransform(const Matrix3x2& transform);
        void PopTransform();

        void DrawLine(const Vector2& start, const Vector2& end, const Color& color, float strokeWidth = 1.0f);
        void DrawRectangle(const Vector2& position, const Vector2& size, const Color& color, float strokeWidth = 1.0f);
        void FillRectangle(const Vector2& position, const Vector2& size, const Color& color);
        void DrawCircle(const Vector2& center, float radius, const Color& color, float strokeWidth = 1.0f);
        void FillCircle(const Vector2& center, float radius, const Color& color);
        void DrawEllipse(const Vector2& center, const Vector2& radius, const Color& color, float strokeWidth = 1.0f);
        void FillEllipse(const Vector2& center, const Vector2& radius, const Color& color);

        void DrawTexture(Texture2D* texture, const Vector2& position, const Vector2& size, 
                        const Color& tint = Color::White, float opacity = 1.0f);
        void DrawTextureRect(Texture2D* texture, const Vector2& position, const Vector2& size,
                           const Vector2& sourcePos, const Vector2& sourceSize,
                           const Color& tint = Color::White, float opacity = 1.0f);
                           
        void DrawSprite(Texture2D* texture, const D2D1_RECT_F& destRect, const D2D1_RECT_F& sourceRect, 
                       const Color& tint = Color::White, float opacity = 1.0f);
                       
        // 새로운 텍스처 시스템용 렌더링 메소드들
        void DrawTexture(std::shared_ptr<Texture> texture, const Vector2& position, const Vector2& size, 
                        const Color& tint = Color::White, float opacity = 1.0f);
        void DrawTextureRect(std::shared_ptr<Texture> texture, const Vector2& position, const Vector2& size,
                           const D2D1_RECT_F& sourceRect, const Color& tint = Color::White, float opacity = 1.0f);

        void DrawText(const std::wstring& text, const Vector2& position, const Color& color,
                     const std::wstring& fontName = L"Arial", float fontSize = 16.0f);
        
        void FillTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Color& color);

        ID2D1DeviceContext6* GetD2DContext() const { return d2dContext.Get(); }
        IDWriteFactory7* GetDWriteFactory() const { return dwriteFactory.Get(); }
        IWICImagingFactory2* GetWICFactory() const { return wicFactory.Get(); }
        
        ID3D11Device* GetD3D11Device() const { return d3dDevice.Get(); }
        ID3D11DeviceContext* GetD3D11DeviceContext() const { return d3dContext.Get(); }
        ID3D11Device* GetD3DDevice() const { return d3dDevice.Get(); }
        ID3D11DeviceContext* GetD3DContext() const { return d3dContext.Get(); }
        
        void Begin3DRender();
        void End3DRender();
        void SetViewMatrix(const DirectX::XMMATRIX& view);
        void SetProjectionMatrix(const DirectX::XMMATRIX& projection);
        
        void DrawTriangle3D();

        int GetWidth() const { return width; }
        int GetHeight() const { return height; }

    private:
        bool CreateDeviceResources();
        bool CreateRenderTarget();
        void ReleaseRenderTarget();
        
        bool InitTriangleResources();
        void ReleaseTriangleResources();
        
        ComPtr<ID2D1SolidColorBrush> CreateSolidBrush(const Color& color);
        ComPtr<IDWriteTextFormat> CreateTextFormat(const std::wstring& fontName, float fontSize);
    };
}