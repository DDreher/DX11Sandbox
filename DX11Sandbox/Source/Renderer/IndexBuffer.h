#pragma once
#include <d3d11.h>

#include "Renderer/Bindable.h"
#include "Renderer/DX11Types.h"

class IndexBuffer : public IBindable
{
public:
    IndexBuffer(uint16* indices, uint32 num_indices);

    void Bind() override;

    inline uint32 GetNum() { return num_; };

    uint32 num_ = 0;
    ComPtr<ID3D11Buffer> index_buffer_;
};
