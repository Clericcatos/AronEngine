#pragma once
#include "../Core/framework.h"
#include "../Utils/Vector2.h"
#include "../Utils/Color.h"
#include <vector>
#include <memory>
#include <unordered_map>

struct ID2D1Effect;
struct ID2D1DeviceContext;
struct ID2D1Bitmap;

namespace AronEngine
{
    enum class VisualEffectType
    {
        Shadow,
        Glow,
        Blur,
        Distortion,
        ColorTint,
        Sepia,
        Brightness,
        Contrast,
        Saturation
    };

    struct VisualEffectParams
    {
        VisualEffectType type;
        Color color;
        float intensity;
        Vector2 offset;
        float blurRadius;
        float distortionAmount;
        bool enabled;

        VisualEffectParams() 
            : type(VisualEffectType::Shadow)
            , color(Color::Black)
            , intensity(1.0f)
            , offset(2.0f, 2.0f)
            , blurRadius(3.0f)
            , distortionAmount(0.1f)
            , enabled(true) 
        {}
    };

    class VisualEffectSystem
    {
    private:
        ID2D1DeviceContext* deviceContext;
        std::unordered_map<VisualEffectType, ID2D1Effect*> effects;
        std::vector<VisualEffectParams> activeEffects;
        bool systemInitialized;

    public:
        VisualEffectSystem();
        ~VisualEffectSystem();

        bool Initialize(ID2D1DeviceContext* context);
        void Shutdown();

        void ApplyEffect(ID2D1Bitmap* inputBitmap, const VisualEffectParams& params, ID2D1Bitmap** outputBitmap);
        void ApplyMultipleEffects(ID2D1Bitmap* inputBitmap, const std::vector<VisualEffectParams>& effectChain, ID2D1Bitmap** outputBitmap);

        ID2D1Effect* CreateShadowEffect(const Color& shadowColor = Color::Black, const Vector2& offset = Vector2(2.0f, 2.0f));
        ID2D1Effect* CreateGlowEffect(const Color& glowColor = Color::White, float intensity = 1.0f);
        ID2D1Effect* CreateBlurEffect(float blurRadius = 3.0f);
        ID2D1Effect* CreateDistortionEffect(float amount = 0.1f);
        ID2D1Effect* CreateColorTintEffect(const Color& tintColor = Color::White);
        
        void SetEffectProperty(VisualEffectType type, const std::string& propertyName, float value);
        void SetEffectProperty(VisualEffectType type, const std::string& propertyName, const Vector2& value);
        void SetEffectProperty(VisualEffectType type, const std::string& propertyName, const Color& value);

        void EnableEffect(VisualEffectType type, bool enable);
        bool IsEffectEnabled(VisualEffectType type) const;

        void AddEffectToChain(const VisualEffectParams& params);
        void RemoveEffectFromChain(VisualEffectType type);
        void ClearEffectChain();

    private:
        bool CreateEffect(VisualEffectType type);
        void CleanupEffects();
    };
}