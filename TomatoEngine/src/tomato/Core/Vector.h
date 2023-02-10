#pragma once

namespace tomato
{
    struct ::ImVec2;
    struct ::ImVec4;

    struct Vec2 : DirectX::SimpleMath::Vector2
    {
        using Vector2::Vector2;

        // ImVec2
        constexpr Vec2(const ImVec2& f)
            : Vec2(f.x, f.y) {}

        operator ImVec2() const { return {x, y}; }
    };

    struct Vec3 : DirectX::SimpleMath::Vector3
    {
        using Vector3::Vector3;

        Vec2 XY() { return {x, y}; }

        explicit operator Vec2() const { return {x, y}; };

        constexpr Vec3(const Vec2& V, float z)
            : Vec3(V.x, V.y, z) {}
    };

    struct Vec4 : DirectX::SimpleMath::Vector4
    {
        using Vector4::Vector4;

        Vec2 XY() { return {x, y}; }
        Vec3 XYZ() { return {x, y, z}; }

        explicit operator Vec2() const { return {x, y}; };
        explicit operator Vec3() const { return {x, y, z}; };

        constexpr Vec4(const Vec2& V, float z, float w)
            : Vec4(V.x, V.y, z, w) {}

        constexpr Vec4(const Vec3& V, float w)
            : Vec4(V.x, V.y, V.z, w) {}


        // ImVec4
        constexpr Vec4(const ImVec4& f)
            : Vec4(f.x, f.y, f.z, f.w) {}

        operator ImVec4() const { return {x, y, z, w}; }
    };

    #pragma region Binary operators
    inline Vec2 operator+(const Vec2& V1, const Vec2& V2) noexcept
    {
        return static_cast<Vector2>(V1) + static_cast<Vector2>(V2);
    }

    inline Vec2 operator-(const Vec2& V1, const Vec2& V2) noexcept
    {
        return static_cast<Vector2>(V1) - static_cast<Vector2>(V2);
    }

    inline Vec2 operator*(const Vec2& V1, const Vec2& V2) noexcept
    {
        return static_cast<Vector2>(V1) * static_cast<Vector2>(V2);
    }

    inline Vec2 operator*(const Vec2& V, float S) noexcept
    {
        return static_cast<Vector2>(V) * S;
    }

    inline Vec2 operator/(const Vec2& V1, const Vec2& V2) noexcept
    {
        return static_cast<Vector2>(V1) / static_cast<Vector2>(V2);
    }

    inline Vec2 operator/(const Vec2& V, float S) noexcept
    {
        return static_cast<Vector2>(V) / S;
    }

    inline Vec2 operator*(float S, const Vec2& V) noexcept
    {
        return S * static_cast<Vector2>(V);
    }

    inline Vec3 operator+(const Vec3& V1, const Vec3& V2) noexcept
    {
        return static_cast<Vector3>(V1) + static_cast<Vector3>(V2);
    }

    inline Vec3 operator-(const Vec3& V1, const Vec3& V2) noexcept
    {
        return static_cast<Vector3>(V1) - static_cast<Vector3>(V2);
    }

    inline Vec3 operator*(const Vec3& V1, const Vec3& V2) noexcept
    {
        return static_cast<Vector3>(V1) * static_cast<Vector3>(V2);
    }

    inline Vec3 operator*(const Vec3& V, float S) noexcept
    {
        return static_cast<Vector3>(V) * S;
    }

    inline Vec3 operator/(const Vec3& V1, const Vec3& V2) noexcept
    {
        return static_cast<Vector3>(V1) / static_cast<Vector3>(V2);
    }

    inline Vec3 operator/(const Vec3& V, float S) noexcept
    {
        return static_cast<Vector3>(V) / S;
    }

    inline Vec3 operator*(float S, const Vec3& V) noexcept
    {
        return S * static_cast<Vector3>(V);
    }

    inline Vec4 operator+(const Vec4& V1, const Vec4& V2) noexcept
    {
        return static_cast<Vector4>(V1) + static_cast<Vector4>(V2);
    }

    inline Vec4 operator-(const Vec4& V1, const Vec4& V2) noexcept
    {
        return static_cast<Vector4>(V1) - static_cast<Vector4>(V2);
    }

    inline Vec4 operator*(const Vec4& V1, const Vec4& V2) noexcept
    {
        return static_cast<Vector4>(V1) * static_cast<Vector4>(V2);
    }

    inline Vec4 operator*(const Vec4& V, float S) noexcept
    {
        return static_cast<Vector4>(V) * S;
    }

    inline Vec4 operator/(const Vec4& V1, const Vec4& V2) noexcept
    {
        return static_cast<Vector4>(V1) / static_cast<Vector4>(V2);
    }

    inline Vec4 operator/(const Vec4& V, float S) noexcept
    {
        return static_cast<Vector4>(V) / S;
    }

    inline Vec4 operator*(float S, const Vec4& V) noexcept
    {
        return S * static_cast<Vector4>(V);
    }

    #pragma endregion
}
