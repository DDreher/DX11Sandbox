#pragma once

class Camera
{
public:
    Camera() = default;
    Camera(Vec3 pos, float aspect_ratio, float fov, float near_clip, float far_clip);

    void SetNearClip(float z);
    void SetFarClip(float z);
    void SetFov(float fov_in_rad);

    const Mat4& GetProjection();
    void SetProjection(const Mat4& projection);

    const Mat4& GetView();
    void SetView(const Mat4& view);

    inline static const Vec3 DEFAULT_POS = Vec3::ZERO;
    inline static const float DEFAULT_ASPECT_RATIO = 16.0f / 9.0f;
    inline static const float DEFAULT_FOV = MathUtils::DegToRad(90.0f);
    inline static const float DEFAULT_NEAR_CLIP = 0.1f;
    inline static const float DEFAULT_FAR_CLIP = 1000.0f;

private:
    void UpdateProjection();
    void UpdateView();

    Vec3 pos_ = Camera::DEFAULT_POS;
    Vec3 up_ = { 0.0f, 1.0f, 0.0f };

    float near_clip_ = Camera::DEFAULT_NEAR_CLIP;
    float far_clip_ = Camera::DEFAULT_FAR_CLIP;
    float fov_ = Camera::DEFAULT_FOV;
    float aspect_ratio_ = Camera::DEFAULT_ASPECT_RATIO;

    Mat4 view_;
    bool is_view_dirty_ = true;

    Mat4 projection_;
    bool is_projection_dirty_ = true;
};
