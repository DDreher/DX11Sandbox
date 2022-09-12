#pragma once
#include "Core/Handle.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/RenderState.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

struct TextureParameter
{
    std::string name;
    uint32 slot = 0;
    Handle<Texture> tex;
};

struct MaterialDesc
{
    std::string vs_path;
    std::string ps_path;
    RasterizerState rasterizer_state;
    BlendState blend_state;
    DepthStencilState depth_stencil_state;
    bool is_alpha_cutoff = false;
    float alpha_cutoff_val = 0.0f;

    bool operator==(const MaterialDesc& other) const
    {
        return vs_path == other.vs_path &&
            ps_path == other.ps_path &&
            rasterizer_state == other.rasterizer_state &&
            blend_state == other.blend_state &&
            depth_stencil_state == other.depth_stencil_state &&
            is_alpha_cutoff == other.is_alpha_cutoff &&
            alpha_cutoff_val == other.alpha_cutoff_val;
    }
};
MAKE_HASHABLE(MaterialDesc, t.vs_path, t.ps_path, t.rasterizer_state, t.blend_state, t.depth_stencil_state,
    t.is_alpha_cutoff, t.alpha_cutoff_val);

class Material
{
public:
    Material(const MaterialDesc& desc);
    ~Material() = default;

    void Bind();

    void SetTexture(const std::string& param_name, Handle<Texture> texture);
    void SetParam(const std::string& param_name, Vec3 val);
    void SetParam(const std::string& param_name, Vec4 val);
    void SetParam(const std::string& param_name, float val);
    void SetParam(const std::string& param_name, int32 val);
    void SetParam(const std::string& param_name, bool val);

    void SetBlendState(const BlendState& state)
    {
        blend_state_ = state;
    }

    void SetDepthStencilState(const DepthStencilState& state)
    {
        depth_stencil_state_ = state;
    }

    void SetRasterizerState(const RasterizerState& state)
    {
        rasterizer_state_ = state;
    }

public:
    Handle<VertexShader> vs_;
    Handle<PixelShader> ps_;

    RasterizerState rasterizer_state_;
    BlendState blend_state_;
    DepthStencilState depth_stencil_state_;

    std::vector<UniquePtr<ConstantBuffer>> cbuffers_;
    std::unordered_map<std::string, TextureParameter> texture_parameters_;
};
