#include "framework.h"
#include "Color.h"
#include <algorithm>

namespace AronEngine
{
    const Color Color::White(1, 1, 1, 1);
    const Color Color::Black(0, 0, 0, 1);
    const Color Color::Red(1, 0, 0, 1);
    const Color Color::Green(0, 1, 0, 1);
    const Color Color::Blue(0, 0, 1, 1);
    const Color Color::Yellow(1, 1, 0, 1);
    const Color Color::Cyan(0, 1, 1, 1);
    const Color Color::Magenta(1, 0, 1, 1);
    const Color Color::Gray(0.5f, 0.5f, 0.5f, 1.0f);
    const Color Color::Clear(0, 0, 0, 0);
    const Color Color::LightGray(0.8f, 0.8f, 0.8f, 1.0f);

    Color Color::FromHSV(float h, float s, float v, float a)
    {
        float c = v * s;
        float x = c * (1 - abs(fmod(h / 60.0f, 2) - 1));
        float m = v - c;

        float r, g, b;
        if (h >= 0 && h < 60) {
            r = c; g = x; b = 0;
        }
        else if (h >= 60 && h < 120) {
            r = x; g = c; b = 0;
        }
        else if (h >= 120 && h < 180) {
            r = 0; g = c; b = x;
        }
        else if (h >= 180 && h < 240) {
            r = 0; g = x; b = c;
        }
        else if (h >= 240 && h < 300) {
            r = x; g = 0; b = c;
        }
        else {
            r = c; g = 0; b = x;
        }

        return Color(r + m, g + m, b + m, a);
    }

    void Color::ToHSV(float& h, float& s, float& v) const
    {
        float max = (std::max)({r, g, b});
        float min = (std::min)({r, g, b});
        float delta = max - min;

        v = max;

        if (delta < 0.00001f)
        {
            s = 0;
            h = 0;
            return;
        }

        if (max > 0)
        {
            s = delta / max;
        }
        else
        {
            s = 0;
            h = 0;
            return;
        }

        if (r >= max)
            h = fmod((g - b) / delta, 6.0f);
        else if (g >= max)
            h = 2.0f + (b - r) / delta;
        else
            h = 4.0f + (r - g) / delta;

        h *= 60.0f;
        if (h < 0)
            h += 360.0f;
    }
}