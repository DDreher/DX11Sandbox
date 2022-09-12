#include "Renderer/IndexBuffer.h"

#include <d3d11.h>

#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"

IndexBuffer::IndexBuffer(uint16* indices, uint32 num_indices)
    : num_(num_indices)
{
    D3D11_BUFFER_DESC index_buffer_desc = {};
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.ByteWidth = sizeof(uint16) * num_indices;
    index_buffer_desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = indices;
    DX11_VERIFY(gfx::device->CreateBuffer(&index_buffer_desc, &subresource_data, &index_buffer_));
}

void IndexBuffer::Bind()
{
    gfx::device_context->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);
}

