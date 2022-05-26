#pragma once
#include "Bindable.h"
#include "DX11Types.h"
#include "GraphicsContext.h"

class IndexBuffer : public IBindable
{
public:
    IndexBuffer(const GraphicsContext& context, uint16* indices, uint32 num_indices);

    void Bind(GraphicsContext& context) override;

    inline uint32 GetNum() { return num_; };

private:
    uint32 num_ = 0;
    ComPtr<ID3D11Buffer> index_buffer_;
};
