#pragma once
#include "Renderer/Bindable.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

struct TextureParameter
{
    std::string name;
    uint32 slot = 0;
    SharedPtr<Texture> tex;
};

struct MaterialDesc
{
    std::string vs_path;
    std::string ps_path;
    RasterizerState rasterizer_state;
    BlendState blend_state;
    DepthStencilState depth_stencil_state;
};

class Material : public IBindable
{
public:
    Material(const MaterialDesc& desc);
    ~Material();

    virtual void Bind(GraphicsContext& context) override;

    void Create(GraphicsContext* context);

    void SetTexture(const std::string& param_name, SharedPtr<Texture> texture);
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
    std::string vs_path_;
    UniquePtr<VertexShader> vs_;
    std::string ps_path_;
    UniquePtr<PixelShader> ps_;

    RasterizerState rasterizer_state_;
    BlendState blend_state_;
    DepthStencilState depth_stencil_state_;

    ComPtr<ID3D11InputLayout> input_layout_ = nullptr;

    std::vector<UniquePtr<ConstantBuffer>> cbuffers_;
    std::unordered_map<std::string, TextureParameter> texture_parameters_;
};
