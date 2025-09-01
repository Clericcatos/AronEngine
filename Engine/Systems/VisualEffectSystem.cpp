#include "framework.h"
#include "VisualEffectSystem.h"
#include <d2d1_3.h>
#include <d2d1effects_2.h>

namespace AronEngine
{
    VisualEffectSystem::VisualEffectSystem()
        : deviceContext(nullptr)
        , systemInitialized(false)
    {
    }

    VisualEffectSystem::~VisualEffectSystem()
    {
        Shutdown();
    }

    bool VisualEffectSystem::Initialize(ID2D1DeviceContext* context)
    {
        if (!context)
        {
            DEBUG_LOG("VisualEffectSystem: Invalid device context");
            return false;
        }

        deviceContext = context;
        deviceContext->AddRef();

        // Create common effects
        CreateEffect(VisualEffectType::Shadow);
        CreateEffect(VisualEffectType::Glow);
        CreateEffect(VisualEffectType::Blur);
        CreateEffect(VisualEffectType::Distortion);
        CreateEffect(VisualEffectType::ColorTint);

        systemInitialized = true;
        DEBUG_LOG("VisualEffectSystem initialized successfully");
        return true;
    }

    void VisualEffectSystem::Shutdown()
    {
        CleanupEffects();
        
        if (deviceContext)
        {
            deviceContext->Release();
            deviceContext = nullptr;
        }
        
        systemInitialized = false;
    }

    void VisualEffectSystem::ApplyEffect(ID2D1Bitmap* inputBitmap, const VisualEffectParams& params, ID2D1Bitmap** outputBitmap)
    {
        if (!systemInitialized || !inputBitmap || !params.enabled) return;

        auto it = effects.find(params.type);
        if (it == effects.end()) return;

        ID2D1Effect* effect = it->second;
        effect->SetInput(0, inputBitmap);

        // Apply effect-specific parameters
        switch (params.type)
        {
            case VisualEffectType::Shadow:
            {
                D2D1_MATRIX_5X4_F shadowMatrix = {
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, params.intensity * params.color.a,
                    0, 0, 0, 0
                };
                effect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, shadowMatrix);
                break;
            }
            case VisualEffectType::Blur:
                effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, params.blurRadius);
                break;
            case VisualEffectType::ColorTint:
            {
                D2D1_MATRIX_5X4_F tintMatrix = {
                    params.color.r * params.intensity, 0, 0, 0,
                    0, params.color.g * params.intensity, 0, 0,
                    0, 0, params.color.b * params.intensity, 0,
                    0, 0, 0, params.color.a, 
                    0, 0, 0, 0
                };
                effect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, tintMatrix);
                break;
            }
        }

        // Get output
        effect->GetOutput(reinterpret_cast<ID2D1Image**>(outputBitmap));
    }

    void VisualEffectSystem::ApplyMultipleEffects(ID2D1Bitmap* inputBitmap, const std::vector<VisualEffectParams>& effectChain, ID2D1Bitmap** outputBitmap)
    {
        if (!systemInitialized || !inputBitmap || effectChain.empty()) return;

        ID2D1Bitmap* currentInput = inputBitmap;
        ID2D1Bitmap* currentOutput = nullptr;

        for (size_t i = 0; i < effectChain.size(); ++i)
        {
            const VisualEffectParams& params = effectChain[i];
            if (!params.enabled) continue;

            ApplyEffect(currentInput, params, &currentOutput);
            
            if (i < effectChain.size() - 1 && currentOutput)
            {
                if (currentInput != inputBitmap)
                {
                    currentInput->Release();
                }
                currentInput = currentOutput;
            }
        }

        *outputBitmap = currentOutput;
    }

    ID2D1Effect* VisualEffectSystem::CreateShadowEffect(const Color& shadowColor, const Vector2& offset)
    {
        if (!deviceContext) return nullptr;

        ID2D1Effect* shadowEffect = nullptr;
        HRESULT hr = deviceContext->CreateEffect(CLSID_D2D1ColorMatrix, &shadowEffect);
        
        if (SUCCEEDED(hr))
        {
            D2D1_MATRIX_5X4_F shadowMatrix = {
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, shadowColor.a,
                0, 0, 0, 0
            };
            shadowEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, shadowMatrix);
        }

        return shadowEffect;
    }

    ID2D1Effect* VisualEffectSystem::CreateGlowEffect(const Color& glowColor, float intensity)
    {
        if (!deviceContext) return nullptr;

        ID2D1Effect* glowEffect = nullptr;
        HRESULT hr = deviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &glowEffect);
        
        if (SUCCEEDED(hr))
        {
            glowEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 3.0f);
        }

        return glowEffect;
    }

    ID2D1Effect* VisualEffectSystem::CreateBlurEffect(float blurRadius)
    {
        if (!deviceContext) return nullptr;

        ID2D1Effect* blurEffect = nullptr;
        HRESULT hr = deviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &blurEffect);
        
        if (SUCCEEDED(hr))
        {
            blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurRadius);
        }

        return blurEffect;
    }

    ID2D1Effect* VisualEffectSystem::CreateDistortionEffect(float amount)
    {
        if (!deviceContext) return nullptr;

        ID2D1Effect* distortionEffect = nullptr;
        HRESULT hr = deviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &distortionEffect);
        
        if (SUCCEEDED(hr))
        {
            D2D1_MATRIX_3X2_F distortionMatrix = {
                1.0f, 0.0f,
                amount, 1.0f,
                0.0f, 0.0f
            };
            distortionEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, distortionMatrix);
        }

        return distortionEffect;
    }

    ID2D1Effect* VisualEffectSystem::CreateColorTintEffect(const Color& tintColor)
    {
        if (!deviceContext) return nullptr;

        ID2D1Effect* tintEffect = nullptr;
        HRESULT hr = deviceContext->CreateEffect(CLSID_D2D1ColorMatrix, &tintEffect);
        
        if (SUCCEEDED(hr))
        {
            D2D1_MATRIX_5X4_F tintMatrix = {
                tintColor.r, 0, 0, 0,
                0, tintColor.g, 0, 0,
                0, 0, tintColor.b, 0,
                0, 0, 0, tintColor.a,
                0, 0, 0, 0
            };
            tintEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, tintMatrix);
        }

        return tintEffect;
    }

    void VisualEffectSystem::EnableEffect(VisualEffectType type, bool enable)
    {
        for (auto& effect : activeEffects)
        {
            if (effect.type == type)
            {
                effect.enabled = enable;
                return;
            }
        }
        
        if (enable)
        {
            VisualEffectParams params;
            params.type = type;
            params.enabled = true;
            activeEffects.push_back(params);
        }
    }

    bool VisualEffectSystem::IsEffectEnabled(VisualEffectType type) const
    {
        for (const auto& effect : activeEffects)
        {
            if (effect.type == type)
            {
                return effect.enabled;
            }
        }
        return false;
    }

    void VisualEffectSystem::AddEffectToChain(const VisualEffectParams& params)
    {
        activeEffects.push_back(params);
    }

    void VisualEffectSystem::RemoveEffectFromChain(VisualEffectType type)
    {
        activeEffects.erase(
            std::remove_if(activeEffects.begin(), activeEffects.end(),
                [type](const VisualEffectParams& params) {
                    return params.type == type;
                }),
            activeEffects.end());
    }

    void VisualEffectSystem::ClearEffectChain()
    {
        activeEffects.clear();
    }

    bool VisualEffectSystem::CreateEffect(VisualEffectType type)
    {
        if (!deviceContext) return false;

        ID2D1Effect* effect = nullptr;
        HRESULT hr = E_FAIL;

        switch (type)
        {
            case VisualEffectType::Shadow:
            case VisualEffectType::ColorTint:
                hr = deviceContext->CreateEffect(CLSID_D2D1ColorMatrix, &effect);
                break;
            case VisualEffectType::Blur:
            case VisualEffectType::Glow:
                hr = deviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &effect);
                break;
            case VisualEffectType::Distortion:
                hr = deviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &effect);
                break;
            default:
                hr = deviceContext->CreateEffect(CLSID_D2D1ColorMatrix, &effect);
                break;
        }

        if (SUCCEEDED(hr) && effect)
        {
            effects[type] = effect;
            return true;
        }

        return false;
    }

    void VisualEffectSystem::CleanupEffects()
    {
        for (auto& pair : effects)
        {
            if (pair.second)
            {
                pair.second->Release();
            }
        }
        effects.clear();
        activeEffects.clear();
    }
}