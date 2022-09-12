#pragma once
#include "Renderer/RenderState.h"

struct TextureDesc
{
    String file_path;

    bool operator==(const TextureDesc& other) const
    {
        return file_path == other.file_path;
    }
};
MAKE_HASHABLE(TextureDesc, t.file_path);

class Texture
{
public:
    Texture(const TextureDesc& desc);
    ~Texture() = default;

    inline bool operator==(const Texture& v) const
    {
        bool b = true;
        b &= this->file_path_ == v.file_path_;
        b &= this->num_channels_ == v.num_channels_;
        b &= this->width_ == v.width_;
        b &= this->height_ == v.height_;
        b &= this->sampler_state_ == v.sampler_state_;
        return b;
    }

    void Create(void* data);

    std::string file_path_;
    int32 num_channels_ = -1;
    int32 width_ = -1;
    int32 height_ = -1;

    SamplerState sampler_state_ = SamplerState::LinearWrap;

    ComPtr<ID3D11Texture2D> handle_ = nullptr;
    ComPtr<ID3D11ShaderResourceView> srv_= nullptr;
};
MAKE_HASHABLE(Texture, t.file_path_, t.num_channels_, t.width_, t.height_);