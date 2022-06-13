#pragma once

DECLSPEC_ALIGN(16)
struct CBufferPerObject
{
    Mat4 mat_world;
    Vec3 color = Vec3(1.0f);
    float padding;
};

DECLSPEC_ALIGN(16)
struct CBufferPerFrame
{
    Mat4 mat_view_projection;
};
