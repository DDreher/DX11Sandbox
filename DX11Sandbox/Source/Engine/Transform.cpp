#include "Engine/Transform.h"

Transform::Transform(const Vec3& scaling, const Quat& rotation, const Vec3& translation)
{
    SetWorldScaling(scaling);
    SetWorldRotation(rotation);
    SetWorldTranslation(translation);
}

void Transform::SetLocalTranslation(const Vec3& translation)
{
    if (translation_local_ != translation)
    {
        translation_local_ = translation;
        RecalculateTransform();
    }
}

const Vec3& Transform::GetLocalScaling() const
{
    return scaling_local_;
}

const Quat& Transform::GetLocalRotation() const
{
    return rotation_local_;
}

const Vec3& Transform::GetLocalTranslation() const
{
    return translation_local_;
}

const Mat4& Transform::GetLocalMatrix() const
{
    return matrix_local_;
}

void Transform::SetWorldScaling(const Vec3& scaling)
{
    scaling_world_ = scaling;
    SetDirty();
}

void Transform::SetWorldRotation(const Quat& rotation)
{
    rotation_world_ = rotation;
    SetDirty();
}

void Transform::SetWorldTranslation(const Vec3& translation)
{
    if(translation_world_ != translation)
    {
        translation_world_ = translation;
        SetDirty();
    }

    if(parent_ != nullptr)
    {
        SetLocalTranslation(translation_world_ * parent_->GetWorldMatrix().Invert());
    }
    else
    {
        SetLocalTranslation(translation_world_);
    }
}

const Vec3& Transform::GetWorldScaling() const
{
    return scaling_world_;
}

const Quat& Transform::GetWorldRotation() const
{
    return rotation_world_;
}

const Vec3& Transform::GetWorldTranslation() const
{
    return translation_world_;
}

const Mat4& Transform::GetWorldMatrix()
{
    matrix_world_ = Mat4::SRT(scaling_world_, rotation_world_, translation_world_);
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
    matrix_local_ = Mat4::SRT(translation_local_, rotation_local_, scaling_local_);
    matrix_world_ = matrix_local_;
    if(parent_ != nullptr)
    {
        matrix_world_ *= parent_->GetWorldMatrix();
    }

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
    }

    parent_ = t;

    SetDirty();

    // TODO: implement properly
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
