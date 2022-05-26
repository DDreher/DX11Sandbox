#pragma once
#include "Bindable.h"
#include "GraphicsContext.h"
#include "Shader.h"
#include "Texture.h"

class Material : public IBindable
{
public:
    virtual void Bind(GraphicsContext& context) override;

    void Create(const GraphicsContext& context);

    void SetTexture(const std::string& param_name, SharedPtr<Texture> texture);
    void SetVec3(const std::string& param_name, Vec3 val);
    void SetVec4(const std::string& param_name, Vec4 val);
    void SetFloat(const std::string& param_name, float val);
    void SetInt(const std::string& param_name, int32 val);
    void SetBool(const std::string& param_name, bool val);

public:
    SharedPtr<Shader<EShaderType::VS>> vs_ = nullptr;
    SharedPtr<Shader<EShaderType::PS>> ps_ = nullptr;

    BlendState blend_state_;
    bool is_two_sided_ = false;

    SharedPtr<Texture> tex_ = nullptr;

    ComPtr<ID3D11InputLayout> input_layout_ = nullptr; // Describes order and type of input data
    ComPtr<ID3D11RasterizerState> rasterizer_state_ = nullptr;
};