#pragma once
#include "Bindable.h"
#include "DX11Types.h"
#include "GraphicsContext.h"

struct VertexBufferSlots
{
    static constexpr uint32 POS = 0;
    static constexpr uint32 NORMALS = 1;
    static constexpr uint32 TEX_COORD = 2;
};

class VertexBuffer : public IBindable
{
public:
    VertexBuffer(const GraphicsContext& context, void* data, uint32 size, size_t bytes_per_element, uint32 slot);

    virtual void Bind(GraphicsContext& context) override;

private:
    uint32 slot_ = 0;
    uint32 stride_ = 0;
    ComPtr<ID3D11Buffer> vertex_buffer_;
};
