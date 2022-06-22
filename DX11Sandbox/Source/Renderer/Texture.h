#pragma once
#include "Renderer/RenderState.h"

class Texture
{
public:
    Texture() = default;
    ~Texture() = default;

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
