#pragma once
#include <d3d11.h>

#include "Renderer/DX11Types.h"

struct VertexBufferSlots
{
    static constexpr uint32 POS = 0;
    static constexpr uint32 NORMALS = 1;
    static constexpr uint32 TEX_COORD = 2;
};

class VertexBuffer
{
public:
    VertexBuffer(void* data, uint32 size, size_t bytes_per_element, uint32 slot);

    void Bind();

    const ComPtr<ID3D11Buffer>& GetNativePtr() const
    {
        return vertex_buffer_;
    }

    uint32 slot_ = 0;
    uint32 stride_ = 0;
    ComPtr<ID3D11Buffer> vertex_buffer_;
};
