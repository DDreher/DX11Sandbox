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
    Texture() = default;
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

    void LoadFromFile(const std::string& asset_path);
    void Create(void* data);

    std::string file_path_;
    int32 num_channels_ = -1;
    int32 width_ = -1;
    int32 height_ = -1;

    SamplerState sampler_state_ = SamplerState::LINEAR_WRAP;

    ComPtr<ID3D11Texture2D> handle_ = nullptr;
    ComPtr<ID3D11ShaderResourceView> srv_= nullptr;
};
MAKE_HASHABLE(Texture, t.file_path_, t.num_channels_, t.width_, t.height_);