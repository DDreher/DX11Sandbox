#pragma once

/**
 * Transform consisting of scale, rotation (quaternion) and translation.
 */
class Transform
{
public:
    Transform();
    Transform(const Vec3& scaling, const Quat& rotation, const Vec3& translation);

    Transform operator*(const Transform& other) const;
    Transform& operator*=(const Transform& other);

    Transform Multiply(const Transform& other) const;

    void SetFromMatrix(const Mat4& matrix);
    void SetWorldTransform(const Transform& t);
    void SetWorldTransform(const Vec3& scaling, const Quat& rotation, const Vec3& translation);
    void SetLocalTransform(const Transform& t);
    void SetLocalTransform(const Vec3& scaling, const Quat& rotation, const Vec3& translation);
    void SetLocalScaling(const Vec3& scaling);
    void SetWorldScaling(const Vec3& scaling);
    void SetLocalRotation(const Quat& rotation);
    void SetWorldRotation(const Quat& rotation);
    void SetLocalTranslation(const Vec3& translation);
    void SetWorldTranslation(const Vec3& translation);

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;

    Vec3 GetLocalScaling() const;
    Quat GetLocalRotation() const;
    Vec3 GetLocalTranslation() const;
    Mat4 GetLocalMatrix() const;
    Vec3 GetWorldScaling() const;
    Quat GetWorldRotation() const;
    Vec3 GetWorldTranslation() const;
    Mat4 GetWorldMatrix() const;

    void SetDirty();
    void RecalculateTransform();

    void SetParent(Transform* t);
    void AddChild(Transform* t);
    void RemoveChild(Transform* t);

private:
    Vec3 scaling_world_ = Vec3(1.0f);
    Quat rotation_world_ = Quat::IDENTITY;
    Vec3 translation_world_ = Vec3::ZERO;
    Mat4 matrix_world_ = Mat4::IDENTITY;

    Vec3 translation_local_ = Vec3::ZERO;
    Vec3 scaling_local_ = Vec3(1.0f);
    Quat rotation_local_ = Quat::IDENTITY;
    Mat4 matrix_local_ = Mat4::IDENTITY;

    bool is_dirty_ = true;

    Transform* parent_ = nullptr;
    std::vector<Transform*> children_;
};
