#pragma once

/**
 * Transform consisting of scale, rotation (quaternion) and translation.
 */
class Transform
{
public:
    Transform() {}
    Transform(const Vec3& scaling, const Quat& rotation, const Vec3& translation);

    void SetLocalScaling(const Vec3& scaling);
    void SetLocalRotation(const Quat& translation);
    void SetLocalTranslation(const Vec3& translation);
    const Vec3& GetLocalScaling() const;
    const Quat& GetLocalRotation() const;
    const Vec3& GetLocalTranslation() const;
    const Mat4& GetLocalMatrix() const;

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;

    void SetWorldScaling(const Vec3& scaling);
    void SetWorldRotation(const Quat& rotation);
    void SetWorldTranslation(const Vec3& translation);

    const Vec3& GetWorldScaling() const;
    const Quat& GetWorldRotation() const;
    const Vec3& GetWorldTranslation() const;
    const Mat4& GetWorldMatrix();

    void SetDirty();
    void RecalculateTransform();

    void SetParent(Transform* t);
    void AddChild(Transform* t);
    void RemoveChild(Transform* t);

private:
    Vec3 scaling_world_ = Vec3(1.0f);
    Quat rotation_world_;
    Vec3 translation_world_;
    Mat4 matrix_world_;

    Vec3 translation_local_ = Vec3::ZERO;
    Vec3 scaling_local_ = Vec3(1.0f);
    Quat rotation_local_ = Quat(0.0f, 0.0f, 0.0f, 1.0f);
    Mat4 matrix_local_;

    bool is_dirty_ = true;

    Transform* parent_ = nullptr;
    std::vector<Transform*> children_;
};
