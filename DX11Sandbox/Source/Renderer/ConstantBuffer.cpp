#include "Renderer/ConstantBuffer.h"

#include "Renderer/GraphicsContext.h"
#include "Renderer/DX11Util.h"

ConstantBuffer::ConstantBuffer(GraphicsContext* context, size_t size)
    : context_(context), size_(size)
{
    Init();
}

//ConstantBuffer::ConstantBuffer(SharedPtr<GraphicsContext> context)
//    : context_(context)
//{
//}

ConstantBuffer::ConstantBuffer(GraphicsContext* context, const CBufferBindingDesc& desc)
    : context_(context), slot_(desc.slot), param_map_(desc.param_map)
{
    Init();
}

ConstantBuffer::~ConstantBuffer()
{
    if(data_ != nullptr)
    {
        delete[] data_;
        data_ = nullptr;
    }
}

void ConstantBuffer::Init()
{
    CHECK(context_ != nullptr);
    if(param_map_.empty() == false)
    {
        CHECK(size_ == 0);
        for (auto [key, val] : param_map_)
        {
            auto it = PARAMETER_TYPE_TO_SIZE_TABLE.find(val.type);
            if(it != PARAMETER_TYPE_TO_SIZE_TABLE.end())
            {
                size_ += it->second;
            }
            else
            {
                CHECK_NO_ENTRY();
            }
        }
    }

    size_ = MathUtils::AlignToBytes(size_, 16);
    CHECK_MSG(size_ % 16 == 0, "CBuffer has to be aligned to 16 byte boundary");

    data_ = new uint8[size_];

    D3D11_BUFFER_DESC buffer_desc = {};
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    buffer_desc.ByteWidth = static_cast<UINT>(size_);
    DX11_VERIFY(context_->device->CreateBuffer(&buffer_desc, nullptr, &buffer_));
}

bool ConstantBuffer::SetFloat(const std::string& param_name, float val)
{
    CHECK(data_ != nullptr);

    auto it = param_map_.find(param_name);
    if(it != param_map_.end())
    {
        const CBufferParam& param = it->second;
        CHECK(param.type == ParameterType::Float);
        std::memcpy(data_ + param.offset, &val, sizeof(float));
        is_dirty_ = true;
        return true;
    }

    return false;
}

bool ConstantBuffer::SetVec3(const std::string& param_name, Vec3 val)
{
    CHECK(data_ != nullptr);

    auto it = param_map_.find(param_name);
    if (it != param_map_.end())
    {
        const CBufferParam& param = it->second;
        CHECK(param.type == ParameterType::Vec3);
        std::memcpy(data_ + param.offset, &val, sizeof(Vec3));
        is_dirty_ = true;
        return true;
    }

    return false;
}

bool ConstantBuffer::SetMat4(const std::string& param_name, Mat4 val)
{
    CHECK(data_ != nullptr);

    auto it = param_map_.find(param_name);
    if (it != param_map_.end())
    {
        const CBufferParam& param = it->second;
        CHECK(param.type == ParameterType::Mat4);
        std::memcpy(data_ + param.offset, &val, sizeof(Mat4));
        is_dirty_ = true;
        return true;
    }

    return false;
}

void ConstantBuffer::SetData(const uint8* data, size_t data_size)
{
    CHECK(data_ != nullptr);
    CHECK(size_ <= data_size);
    CHECK(data_size > 0);

    std::memcpy(data_, data, data_size);
    is_dirty_ = true;
}

void ConstantBuffer::Upload(const uint8* data, size_t data_size)
{
    CHECK(data_ != nullptr);
    CHECK(size_ <= data_size);
    CHECK(data_size > 0);
    SetData(data, data_size);
    Upload();
}

void ConstantBuffer::Upload()
{
    CHECK(context_ != nullptr);
    CHECK(buffer_ != nullptr);
    CHECK(data_ != nullptr);

    if(is_dirty_)
    {
        context_->device_context->UpdateSubresource(buffer_.Get(), 0, nullptr, (void*) data_, 0, 0);
        is_dirty_ = false;
    }
}
