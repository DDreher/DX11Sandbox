#include "Maths.h"

const Vec2 Vec2::ZERO = { 0.0f, 0.0f };
const Vec2 Vec2::ONE = { 1.0f, 1.0f };

const Vec3 Vec3::ZERO = { 0.0f, 0.0f, 0.0f };
const Vec3 Vec3::ONE = { 1.0f, 1.0f, 1.0f };
const Vec3 Vec3::UP = { 0.0f, 1.0f, 0.0f };
const Vec3 Vec3::DOWN = { 0.0f, -1.0f, 0.0f };
const Vec3 Vec3::FORWARD = { 0.0f, 0.0f, 1.0f };
const Vec3 Vec3::BACKWARD = { 0.0f, 0.0f, -1.0f };
const Vec3 Vec3::RIGHT = { 1.0f, 0.0f, 0.0f };
const Vec3 Vec3::LEFT = { -1.0f, 0.0f, 0.0f };

const Quat Quat::IDENTITY = { 0.f, 0.f, 0.f, 1.f };

//////////////////////////////////////////////////////////////////////////

Vec3 operator*(const Vec3& v, const Mat4& m)
{
    using namespace DirectX;
    const XMVECTOR vec = XMLoadFloat3(&v);
    const XMMATRIX mat = XMLoadFloat4x4(&m);
    const XMVECTOR result = XMVector3TransformCoord(vec, mat);

    Vec3 out;
    XMStoreFloat3(&out, result);
    return result;
}

Vec3 Vec3::Cross(const Vec3& v) const
{
    using namespace DirectX;
    const XMVECTOR xm_v1 = XMLoadFloat3(this);
    const XMVECTOR xm_v2 = XMLoadFloat3(&v);
    const XMVECTOR xm_out = XMVector3Cross(xm_v1, xm_v2);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

float Vec3::Dot(const Vec3& v) const
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadFloat3(this);
    const XMVECTOR v2 = XMLoadFloat3(&v);
    const XMVECTOR result = XMVector3Dot(v1, v2);
    return XMVectorGetX(result);
}

float Vec3::Length() const
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadFloat3(this);
    const XMVECTOR result = XMVector3Length(v1);
    return XMVectorGetX(result);
}

float Vec3::LengthSquared() const
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadFloat3(this);
    const XMVECTOR result = XMVector3LengthSq(v1);
    return XMVectorGetX(result);
}

float Vec3::Distance(const Vec3& v1, const Vec3& v2)
{
    using namespace DirectX;
    const XMVECTOR a = XMLoadFloat3(&v1);
    const XMVECTOR b = XMLoadFloat3(&v2);
    const XMVECTOR b_minus_a = XMVectorSubtract(b, a);
    const XMVECTOR length = XMVector3Length(b_minus_a);
    return XMVectorGetX(length);
}

float Vec3::DistanceSquared(const Vec3& v1, const Vec3& v2)
{
    using namespace DirectX;
    const XMVECTOR a = XMLoadFloat3(&v1);
    const XMVECTOR b = XMLoadFloat3(&v2);
    const XMVECTOR b_minus_a = XMVectorSubtract(b, a);
    const XMVECTOR length_squared = XMVector3LengthSq(b_minus_a);
    return XMVectorGetX(length_squared);
}

Vec3 Vec3::Cross(const Vec3& v1, const Vec3& v2)
{
    using namespace DirectX;
    const XMVECTOR xm_v1 = XMLoadFloat3(&v1);
    const XMVECTOR xm_v2 = XMLoadFloat3(&v2);
    const XMVECTOR xm_out = XMVector3Cross(xm_v1, xm_v2);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

Vec3 Vec3::Transform(const Vec3& v, const Quat& q)
{
    using namespace DirectX;
    const XMVECTOR xm_v = XMLoadFloat3(&v);
    const XMVECTOR xm_q = XMLoadFloat4(&q);
    const XMVECTOR xm_out = XMVector3Rotate(xm_v, xm_q);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

//////////////////////////////////////////////////////////////////////////

Mat4& Mat4::operator*=(const Mat4& other)
{
    using namespace DirectX;
    const XMMATRIX m1 = XMLoadFloat4x4(this);
    const XMMATRIX m2 = XMLoadFloat4x4(&other);
    const XMMATRIX multiplied = XMMatrixMultiply(m1, m2);
    XMStoreFloat4x4(this, multiplied);
    return *this;
}

void Mat4::Decompose(Vec3& out_scaling, Quat& out_rotation, Vec3& out_translation) const
{
    using namespace DirectX;

    XMVECTOR scaling;
    XMVECTOR rotation;
    XMVECTOR translation;
    bool success = XMMatrixDecompose(&scaling, &rotation, &translation, *this);
    CHECK(success);

    XMStoreFloat3(&out_scaling, scaling);
    XMStoreFloat4(&out_rotation, rotation);
    XMStoreFloat3(&out_translation, translation);
}

Mat4 Mat4::Transpose() const
{
    DirectX::XMMATRIX mat = *this;
    return Mat4(DirectX::XMMatrixTranspose(mat));
}

Mat4 Mat4::Invert() const
{
    using namespace DirectX;
    XMMATRIX mat = *this;
    XMVECTOR det;
    return DirectX::XMMatrixInverse(&det, mat);
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

Mat4 Mat4::SRT(const Vec3& scaling, const Quat& rotation, const Vec3& translation)
{
    return Mat4::Scaling(scaling) * rotation.ToMatrix() * Mat4::Translation(translation);
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

//////////////////////////////////////////////////////////////////////////

void Quat::Normalize()
{
    using namespace DirectX;
    const XMVECTOR q = XMLoadFloat4(this);
    XMStoreFloat4(this, XMQuaternionNormalize(q));
}

Quat Quat::Normalize(const Quat& q)
{
    using namespace DirectX;
    const XMVECTOR xm_q = XMLoadFloat4(&q);

    Quat out;
    XMStoreFloat4(&out, XMQuaternionNormalize(xm_q));
    return out;
}

Quat Quat::Inverse()
{
    using namespace DirectX;
    const XMVECTOR q = XMLoadFloat4(this);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionInverse(q));
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
