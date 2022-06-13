#include "Camera.h"

Camera::Camera(Vec3 pos, float aspect_ratio, float fov, float near_clip, float far_clip)
    : pos_(pos), aspect_ratio_(aspect_ratio), fov_(fov), near_clip_(near_clip), far_clip_(far_clip)
{
    Vec3 target = { 0.0f, 0.0f, 0.0f };
    LookAt(target);
    view_ = Mat4::LookAt(pos_, target, up_);
    UpdateMatrices();
}

void Camera::LookAt(Vec3 target)
{
    SetView(Mat4::LookAt(pos_, target, up_));
}

void Camera::SetNearClip(float z)
{
    near_clip_ = z;
    is_projection_dirty_ = true;
}

void Camera::SetFarClip(float z)
{
    far_clip_ = z;
    is_projection_dirty_ = true;
}

void Camera::SetFov(float fov_in_rad)
{
    fov_ = fov_in_rad;
    is_projection_dirty_ = true;
}

const Mat4& Camera::GetProjection()
{
    if(is_projection_dirty_)
    {
        UpdateProjection();
    }

    return projection_;
}

void Camera::SetProjection(const Mat4& projection)
{
    projection_ = projection;
}

const Mat4& Camera::GetView()
{
    if (is_view_dirty_)
    {
        UpdateView();
    }

    return view_;
}

void Camera::SetView(const Mat4& view)
{
    view_ = view;
    is_view_dirty_ = true;
}

const Mat4& Camera::GetViewProjection()
{
    return view_projection_;
}

void Camera::SetViewProjection(const Mat4& vp)
{
    view_projection_ = vp;
}

void Camera::UpdateMatrices()
{
    bool is_vp_dirty = is_view_dirty_ || is_projection_dirty_;

    UpdateProjection();
    UpdateView();

    if(is_vp_dirty)
    {
        UpdateViewProjection();
    }
}

void Camera::UpdateProjection()
{
    projection_ = Mat4::PerspectiveFovLH(fov_, aspect_ratio_, near_clip_, far_clip_);
    is_projection_dirty_ = false;
}

void Camera::UpdateView()
{
    // TODO: Maybe we want to automatically track a target at some point...?
    is_view_dirty_ = false;
}

void Camera::UpdateViewProjection()
{
    view_projection_ = view_ * projection_;
}
