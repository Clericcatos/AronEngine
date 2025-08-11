#pragma once
#include "Vector2.h"
#include <cmath>

namespace AronEngine
{
    struct Matrix3x2
    {
        float m11, m12;
        float m21, m22;
        float m31, m32;

        Matrix3x2()
            : m11(1), m12(0)
            , m21(0), m22(1)
            , m31(0), m32(0)
        {}

        Matrix3x2(float m11, float m12, float m21, float m22, float m31, float m32)
            : m11(m11), m12(m12)
            , m21(m21), m22(m22)
            , m31(m31), m32(m32)
        {}

        static Matrix3x2 Identity()
        {
            return Matrix3x2(1, 0, 0, 1, 0, 0);
        }

        static Matrix3x2 Translation(float x, float y)
        {
            return Matrix3x2(1, 0, 0, 1, x, y);
        }

        static Matrix3x2 Translation(const Vector2& position)
        {
            return Translation(position.x, position.y);
        }

        static Matrix3x2 Scale(float x, float y)
        {
            return Matrix3x2(x, 0, 0, y, 0, 0);
        }

        static Matrix3x2 Scale(float scale)
        {
            return Scale(scale, scale);
        }

        static Matrix3x2 Scale(const Vector2& scale)
        {
            return Scale(scale.x, scale.y);
        }

        static Matrix3x2 Rotation(float degrees)
        {
            float rad = degrees * 0.01745329f;
            float cos = std::cos(rad);
            float sin = std::sin(rad);
            return Matrix3x2(cos, sin, -sin, cos, 0, 0);
        }

        static Matrix3x2 TRS(const Vector2& position, float rotation, const Vector2& scale)
        {
            return Translation(position) * Rotation(rotation) * Scale(scale);
        }

        Matrix3x2 operator*(const Matrix3x2& other) const
        {
            return Matrix3x2(
                m11 * other.m11 + m12 * other.m21,
                m11 * other.m12 + m12 * other.m22,
                m21 * other.m11 + m22 * other.m21,
                m21 * other.m12 + m22 * other.m22,
                m31 * other.m11 + m32 * other.m21 + other.m31,
                m31 * other.m12 + m32 * other.m22 + other.m32
            );
        }

        Vector2 TransformPoint(const Vector2& point) const
        {
            return Vector2(
                point.x * m11 + point.y * m21 + m31,
                point.x * m12 + point.y * m22 + m32
            );
        }

        Vector2 TransformVector(const Vector2& vector) const
        {
            return Vector2(
                vector.x * m11 + vector.y * m21,
                vector.x * m12 + vector.y * m22
            );
        }

        Matrix3x2 Inverse() const
        {
            float det = m11 * m22 - m12 * m21;
            if (std::abs(det) < 0.0001f)
                return Identity();

            float invDet = 1.0f / det;
            return Matrix3x2(
                m22 * invDet,
                -m12 * invDet,
                -m21 * invDet,
                m11 * invDet,
                (m21 * m32 - m22 * m31) * invDet,
                (m12 * m31 - m11 * m32) * invDet
            );
        }
    };
}