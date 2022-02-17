#include "VertexBuffer.h"

#include "DX11Util.h"

VertexBuffer::VertexBuffer(const GraphicsContext& context, void* data, uint32 size, size_t bytes_per_element, uint32 slot)
    : stride_((uint32) bytes_per_element),
    slot_(slot)
{
    D3D11_BUFFER_DESC vertex_buffer_desc = {};
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.ByteWidth = (uint32) bytes_per_element * size;
    vertex_buffer_desc.CPUAccessFlags = 0;

    // Fill info about the initial data of the vertex buffer
    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = data;
    DX11_VERIFY(context.device->CreateBuffer(&vertex_buffer_desc, &subresource_data, &vertex_buffer_));
}

void VertexBuffer::Bind(GraphicsContext& context)
{
    static const uint32 offset = 0; // Hardcoded for now...

    context.device_context->IASetVertexBuffers(
        slot_,  // Slot we bind it to
        1,  // Num buffers
        vertex_buffer_.GetAddressOf(),    // pointer to the buffer
        &stride_,    // size of each vertex
        &offset     // offset into the buffer
    );
}
