#pragma once
#include <DirectXMath.h>

struct MathUtils
{
    static float DegToRad(float f)
    {
        return DirectX::XMConvertToRadians(f);
    }
};

struct Vec2 : public DirectX::XMFLOAT2
{
    Vec2() : DirectX::XMFLOAT2(0.0f, 0.0f) {}
    Vec2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
    Vec2(DirectX::XMVECTOR v) { DirectX::XMStoreFloat2(this, v); }

    operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat2(this); }
};

struct Vec3 : public DirectX::XMFLOAT3
{
    Vec3() : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) {}
    Vec3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
    Vec3(DirectX::XMVECTOR v) { DirectX::XMStoreFloat3(this, v); }

    operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat3(this); }

    static const Vec3 ZERO;
    static const Vec3 ONE;
    static const Vec3 UP;
    static const Vec3 DOWN;
    static const Vec3 FORWARD;
    static const Vec3 BACKWARD;
    static const Vec3 RIGHT;
    static const Vec3 LEFT;
};

struct Vec4 : public DirectX::XMFLOAT4
{
    Vec4() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) {}
    Vec4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    Vec4(DirectX::XMVECTOR v) { DirectX::XMStoreFloat4(this, v); }

    operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat4(this); }
};

struct Mat4 : public DirectX::XMFLOAT4X4
{
    Mat4() = default;
    Mat4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13, 
        float m20, float m21, float m22, float m23, 
        float m30, float m31, float m32, float m33)
        : DirectX::XMFLOAT4X4(m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33)
    {}

    Mat4(const DirectX::XMFLOAT4X4& other) { memcpy(this, &other, sizeof(DirectX::XMFLOAT4X4)); }
    Mat4(const DirectX::XMMATRIX& other) { DirectX::XMStoreFloat4x4(this, other); }

    operator DirectX::XMMATRIX() const { return DirectX::XMLoadFloat4x4(this); }

    Mat4 Transpose() const;

    static Mat4 Translation(const Vec3& v);
    static Mat4 Translation(float x, float y, float z);
    static Mat4 RotationX(float rad);
    static Mat4 RotationY(float rad);
    static Mat4 RotationZ(float rad);
    static Mat4 Scaling(float s);
    static Mat4 Scaling(const Vec3& v);
    static Mat4 Scaling(float x, float y, float z);

    static Mat4 LookAt(const Vec3& origin, const Vec3& target, const Vec3& up);
    static Mat4 PerspectiveFovLH(float fov_y_rad, float aspect_ratio, float near_z, float far_z);

    static const Mat4 IDENTITY;
};

Mat4 operator*(const Mat4& mat_a, const Mat4& mat_b);
