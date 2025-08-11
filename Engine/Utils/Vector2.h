#pragma once
#include <cmath>

namespace AronEngine
{
    struct Vector2
    {
        float x, y;

        Vector2() : x(0), y(0) {}
        Vector2(float x, float y) : x(x), y(y) {}
        Vector2(const Vector2& other) : x(other.x), y(other.y) {}

        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 Up;
        static const Vector2 Down;
        static const Vector2 Left;
        static const Vector2 Right;

        Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
        Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
        Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
        Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }
        Vector2 operator-() const { return Vector2(-x, -y); }

        Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
        Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
        Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
        Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

        bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Vector2& other) const { return !(*this == other); }

        float Magnitude() const { return std::sqrt(x * x + y * y); }
        float Length() const { return Magnitude(); }
        float SqrMagnitude() const { return x * x + y * y; }
        float LengthSquared() const { return SqrMagnitude(); }
        
        Vector2 Normalized() const
        {
            float mag = Magnitude();
            if (mag > 0)
                return *this / mag;
            return Zero;
        }

        void Normalize()
        {
            float mag = Magnitude();
            if (mag > 0)
            {
                x /= mag;
                y /= mag;
            }
        }

        float Dot(const Vector2& other) const { return x * other.x + y * other.y; }
        float Cross(const Vector2& other) const { return x * other.y - y * other.x; }
        
        static float Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
        static float Cross(const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; }
        static float Distance(const Vector2& a, const Vector2& b) { return (b - a).Magnitude(); }
        static float SqrDistance(const Vector2& a, const Vector2& b) { return (b - a).SqrMagnitude(); }
        static Vector2 Lerp(const Vector2& a, const Vector2& b, float t) { return a + (b - a) * t; }
        
        static float Angle(const Vector2& from, const Vector2& to)
        {
            float dot = Dot(from.Normalized(), to.Normalized());
            return acos((std::max)(-1.0f, (std::min)(1.0f, dot))) * 57.29578f;
        }

        static Vector2 Rotate(const Vector2& v, float degrees)
        {
            float rad = degrees * 0.01745329f;
            float cos = std::cos(rad);
            float sin = std::sin(rad);
            return Vector2(v.x * cos - v.y * sin, v.x * sin + v.y * cos);
        }
    };

    inline Vector2 operator*(float scalar, const Vector2& v) { return v * scalar; }
}