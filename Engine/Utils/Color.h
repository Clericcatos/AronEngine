#pragma once

namespace AronEngine
{
    struct Color
    {
        float r, g, b, a;

        Color() : r(0), g(0), b(0), a(1) {}
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        Color(int r, int g, int b, int a = 255) 
            : r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(a / 255.0f) {}

        static const Color White;
        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Gray;
        static const Color Clear;
        static const Color LightGray;

        Color operator+(const Color& other) const 
        { 
            return Color(r + other.r, g + other.g, b + other.b, a + other.a); 
        }
        
        Color operator-(const Color& other) const 
        { 
            return Color(r - other.r, g - other.g, b - other.b, a - other.a); 
        }
        
        Color operator*(float scalar) const 
        { 
            return Color(r * scalar, g * scalar, b * scalar, a * scalar); 
        }
        
        Color operator*(const Color& other) const 
        { 
            return Color(r * other.r, g * other.g, b * other.b, a * other.a); 
        }

        static Color Lerp(const Color& a, const Color& b, float t)
        {
            return Color(
                a.r + (b.r - a.r) * t,
                a.g + (b.g - a.g) * t,
                a.b + (b.b - a.b) * t,
                a.a + (b.a - a.a) * t
            );
        }

        static Color FromHSV(float h, float s, float v, float a = 1.0f);
        void ToHSV(float& h, float& s, float& v) const;
    };
}