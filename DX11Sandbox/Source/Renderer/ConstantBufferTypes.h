#pragma once

DECLSPEC_ALIGN(16)
struct CBufferPerObject
{
    Mat4 mat_world;
};

DECLSPEC_ALIGN(16)
struct CBufferPerView
{
    Mat4 mat_view;
    Mat4 mat_inv_view;
    Mat4 mat_view_projection;
    Mat4 mat_inv_view_projection;
    Vec4 pos_camera_ws;
};

DECLSPEC_ALIGN(16)
struct CBufferPerFrame
{
};
