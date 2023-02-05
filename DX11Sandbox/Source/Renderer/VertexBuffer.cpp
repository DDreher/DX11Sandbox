#include "Renderer/VertexBuffer.h"

#include <d3d11.h>

#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"

VertexBuffer::VertexBuffer(void* data, uint32 size, size_t bytes_per_element, uint32 slot)
    : stride_((uint32) bytes_per_element),
    slot_(slot)
{
    CHECK(size > 0);
    D3D11_BUFFER_DESC vertex_buffer_desc = {};
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.ByteWidth = (uint32) bytes_per_element * size;
    vertex_buffer_desc.CPUAccessFlags = 0;

    // Fill info about the initial data of the vertex buffer
    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = data;
    DX11_VERIFY(gfx::device->CreateBuffer(&vertex_buffer_desc, &subresource_data, &vertex_buffer_));
}

void VertexBuffer::Bind()
{
    static const uint32 offset = 0; // Hardcoded for now...

    gfx::device_context->IASetVertexBuffers(
        slot_,  // Slot we bind it to
        1,  // Num buffers
        vertex_buffer_.GetAddressOf(),    // pointer to the buffer
        &stride_,    // size of each vertex
        &offset     // offset into the buffer
    );
}
