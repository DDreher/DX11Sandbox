#include "Maths.h"

const Vec3 Vec3::ZERO = { 0.0f, 0.0f, 0.0f };
const Vec3 Vec3::ONE = { 1.0f, 1.0f, 1.0f };
const Vec3 Vec3::UP = { 0.0f, 1.0f, 0.0f };
const Vec3 Vec3::DOWN = { 0.0f, -1.0f, 0.0f };
const Vec3 Vec3::FORWARD = { 0.0f, 0.0f, 1.0f };
const Vec3 Vec3::BACKWARD = { 0.0f, 0.0f, -1.0f };
const Vec3 Vec3::RIGHT = { 1.0f, 0.0f, 0.0f };
const Vec3 Vec3::LEFT = { -1.0f, 0.0f, 0.0f };

//////////////////////////////////////////////////////////////////////////

Mat4 Mat4::Transpose() const
{
    DirectX::XMMATRIX mat = *this;
    return Mat4(DirectX::XMMatrixTranspose(mat));
}

Mat4 Mat4::Translation(const Vec3& v)
{
    return DirectX::XMMatrixTranslation(v.x, v.y, v.z);
}

Mat4 Mat4::Translation(float x, float y, float z)
{
    return DirectX::XMMatrixTranslation(x, y, z);
}

Mat4 Mat4::RotationX(float rad)
{
    return DirectX::XMMatrixRotationX(rad);
}

Mat4 Mat4::RotationY(float rad)
{
    return DirectX::XMMatrixRotationZ(rad);
}

Mat4 Mat4::RotationZ(float rad)
{
    return DirectX::XMMatrixRotationZ(rad);
}

Mat4 Mat4::Scaling(float s)
{
    return DirectX::XMMatrixScaling(s, s, s);
}

Mat4 Mat4::Scaling(const Vec3& v)
{
    return DirectX::XMMatrixScaling(v.x, v.y, v.z);
}

Mat4 Mat4::Scaling(float x, float y, float z)
{
    return DirectX::XMMatrixScaling(x, y, z);
}

Mat4 Mat4::LookAt(const Vec3& origin, const Vec3& target, const Vec3& up)
{
    return DirectX::XMMatrixLookAtLH(origin, target, up);
}

Mat4 Mat4::PerspectiveFovLH(float fov_y_rad, float aspect_ratio, float near_z, float far_z)
{
    return DirectX::XMMatrixPerspectiveFovLH(fov_y_rad, aspect_ratio, near_z, far_z);
}

const Mat4 Mat4::IDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f };

Mat4 operator*(const Mat4& mat_a, const Mat4& mat_b)
{
    return DirectX::XMMatrixMultiply(mat_a, mat_b);
}

void Quat::Normalize()
{
    using namespace DirectX;
    const XMVECTOR q = XMLoadFloat4(this);
    XMStoreFloat4(this, XMQuaternionNormalize(q));
}

Quat Quat::Inverse()
{
    using namespace DirectX;
    const XMVECTOR q = XMLoadFloat4(this);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionInverse(q));
    return out;
}

Vec3 Quat::ToEuler() const
{
    Vec3 out;

    // TODO

    return out;
}

Mat4 Quat::ToMatrix() const
{
    using namespace DirectX;
    const XMVECTOR quat = XMLoadFloat4(this);
    Mat4 out;
    XMStoreFloat4x4(&out, XMMatrixRotationQuaternion(quat));
    return out;
}

Quat Quat::FromAxisAngle(const Vec3& axis, float angle)
{
    using namespace DirectX;
    const XMVECTOR axis_vec = XMLoadFloat3(&axis);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationAxis(axis_vec, angle));
    return out;
}

Quat Quat::FromPitchYawRoll(float pitch, float yaw, float roll)
{
    using namespace DirectX;
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
    return out;
}

Quat Quat::FromPitchYawRoll(const Vec3& v)
{
    using namespace DirectX;
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationRollPitchYawFromVector(v));
    return out;
}

Quat Quat::FromRotationMatrix(const Mat4& m)
{
    using namespace DirectX;
    const XMMATRIX mat = XMLoadFloat4x4(&m);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationMatrix(mat));
    return out;
}

Quat Quat::Lerp(const Quat& a, const Quat& b, float t)
{
    Quat out;
    return out;
}

const Quat Quat::IDENTITY;
