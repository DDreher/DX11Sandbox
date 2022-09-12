#include "Engine/Transform.h"

Transform::Transform()
{
}

Transform::Transform(const Vec3& scaling, const Quat& rotation, const Vec3& translation)
{
    SetWorldScaling(scaling);
    SetWorldRotation(rotation);
    SetWorldTranslation(translation);
}

Transform Transform::operator*(const Transform& other) const
{
    Mat4 mat = matrix_world_ * other.GetWorldMatrix();
    Vec3 scaling;
    Quat rotation;
    Vec3 translation;
    mat.Decompose(scaling, rotation, translation);
    return {
        scaling, rotation, translation
    };
}

Transform& Transform::operator*=(const Transform& other)
{
    matrix_world_ *= other.GetWorldMatrix();

    SetDirty();
    RecalculateTransform();

    return *this;
}

void Transform::SetFromMatrix(const Mat4& matrix)
{
    Vec3 scaling;
    Quat rotation;
    Vec3 translation;
    matrix.Decompose(scaling, rotation, translation);
    SetWorldScaling(scaling);
    SetWorldRotation(rotation);
    SetWorldTranslation(translation);
}

void Transform::SetWorldTransform(const Transform& t)
{
    SetWorldTransform(t.GetWorldScaling(), t.GetWorldRotation(), t.GetWorldTranslation());
}

void Transform::SetWorldTransform(const Vec3& scaling, const Quat& rotation, const Vec3& translation)
{
    if (scaling_world_ != scaling)
    {
        scaling_world_ = scaling;
        SetDirty();
    }

    if (rotation_world_ != rotation)
    {
        rotation_world_ = rotation;
        SetDirty();
    }

    if (translation_world_ != translation)
    {
        translation_world_ = translation;
        SetDirty();
    }

    RecalculateTransform();
}

void Transform::SetLocalTransform(const Transform& t)
{
    SetLocalTransform(t.GetLocalScaling(), t.GetLocalRotation(), t.GetLocalTranslation());
}

void Transform::SetLocalTransform(const Vec3& scaling, const Quat& rotation, const Vec3& translation)
{
    if (scaling_local_ != scaling)
    {
        scaling_local_ = scaling;
        SetDirty();
    }

    if (rotation_local_ != rotation)
    {
        rotation_local_ = rotation;
        SetDirty();
    }

    if (translation_local_ != translation)
    {
        translation_local_ = translation;
        SetDirty();
    }
    
    RecalculateTransform();
}

void Transform::SetLocalScaling(const Vec3& scaling)
{
    if (scaling_local_ != scaling)
    {
        scaling_local_ = scaling;
        SetDirty();
        RecalculateTransform();
    }
}

void Transform::SetWorldScaling(const Vec3& scaling)
{
    if (scaling_world_ != scaling)
    {
        scaling_world_ = scaling;
        SetDirty();

        if (parent_ == nullptr)
        {
            SetLocalScaling(scaling);
        }
        else
        {
            SetLocalScaling(scaling / parent_->GetWorldScaling());
        }
    }
}

void Transform::SetLocalRotation(const Quat& rotation)
{
    if (rotation_local_ != rotation)
    {
        rotation_local_ = rotation;
        SetDirty();
        RecalculateTransform();
    }
}

void Transform::SetWorldRotation(const Quat& rotation)
{
    if (rotation_world_ != rotation)
    {
        rotation_world_ = rotation;
        SetDirty();

        if (parent_ == nullptr)
        {
            SetLocalRotation(rotation);
        }
        else
        {
            SetLocalRotation(rotation * parent_->GetWorldRotation().Inverse());
        }
    }
}

void Transform::SetLocalTranslation(const Vec3& translation)
{
    if (translation_local_ != translation)
    {
        translation_local_ = translation;
        SetDirty();
        RecalculateTransform();
    }
}

void Transform::SetWorldTranslation(const Vec3& translation)
{
    if (translation_world_ != translation)
    {
        translation_world_ = translation;
        SetDirty();

        if (parent_ == nullptr)
        {
            SetLocalTranslation(translation);
        }
        else
        {
            SetLocalTranslation(translation * parent_->GetWorldMatrix().Invert());
        }
    }
}

Vec3 Transform::GetLocalScaling() const
{
    return scaling_local_;
}

Quat Transform::GetLocalRotation() const
{
    return rotation_local_;
}

Vec3 Transform::GetLocalTranslation() const
{
    return translation_local_;
}

Mat4 Transform::GetLocalMatrix() const
{
    return matrix_local_;
}

Vec3 Transform::GetWorldScaling() const
{
    return scaling_world_;
}

Quat Transform::GetWorldRotation() const
{
    return rotation_world_;
}

Vec3 Transform::GetWorldTranslation() const
{
    return translation_world_;
}

Mat4 Transform::GetWorldMatrix() const
{
    return matrix_world_;
}

void Transform::SetDirty()
{
    is_dirty_ = true;
}

void Transform::RecalculateTransform()
{
    if (is_dirty_ == false)
    {
        return;
    }

    // First recalculate this transform
    matrix_local_ = Mat4::SRT(scaling_local_, rotation_local_, translation_local_);
    matrix_world_ = matrix_local_;
    if(parent_ != nullptr)
    {
        matrix_world_ *= parent_->GetWorldMatrix();
    }

    matrix_local_.Decompose(scaling_local_, rotation_local_, translation_local_);
    matrix_world_.Decompose(scaling_world_, rotation_world_, translation_world_);

    // Then its children
    for(Transform* t : children_)
    {
        t->RecalculateTransform();
    }

    is_dirty_ = false;
}

void Transform::SetParent(Transform* t)
{
    CHECK(t != this);

    if(parent_ != nullptr)
    {
        // TODO: Remove from previous parent
        CHECK_NO_ENTRY();
    }

    parent_ = t;

    SetDirty();
    RecalculateTransform();
}

void Transform::AddChild(Transform* t)
{
    CHECK(t != nullptr);
    CHECK(t != this);
    children_.push_back(t);
    t->SetParent(this);
}

void Transform::RemoveChild(Transform* t)
{
    CHECK(t != nullptr);
    CHECK(t != this);

    children_.erase(remove_if(children_.begin(), children_.end(), [t](Transform* child_transform) 
        {
            return child_transform == t;
        }),
        children_.end());
    t->SetParent(nullptr);
}
