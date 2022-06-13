#pragma once

#include <d3d11shader.h>

#include "Core/FileIO.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/Bindable.h"
#include "Renderer/Vertex.h"

class ConstantBuffer;

struct ShaderCompiler
{
    static HRESULT Compile(const std::string& asset_path, const std::vector<char>& shader_bytes, const char* entry_point,
        const char* shader_target, ComPtr<ID3DBlob>& out_shader_blob);
};

enum class EShaderType
{
    VS,
    PS,
    CS,
    DS,
    GS,
    HS,
};

struct TextureBindingDesc
{
    std::string name;
    UINT slot;
};

class ShaderBase : public IBindable
{
    friend class Material;

public:
    ShaderBase(GraphicsContext* context, const std::string& asset_path, EShaderType shader_type);
    virtual ~ShaderBase();

    bool LoadFromFile(const std::string asset_path);
    bool Compile(const GraphicsContext& context, const std::vector<char>& bytes);
    virtual void Create(GraphicsContext& context) {};

protected:
    virtual void Reflect(GraphicsContext& context);

    EShaderType shader_type_;
    std::string asset_path_;
    ComPtr<ID3DBlob> shader_blob_;
    ComPtr<ID3D11ShaderReflection> shader_reflection_;
    D3D11_SHADER_DESC shader_desc_;

    std::vector<char> shader_code_;
    GraphicsContext* context_ = nullptr;
    //std::vector<SharedPtr<ConstantBuffer>> constant_buffers_;

    std::vector<CBufferBindingDesc> cbuffer_bindings_;
    std::vector<TextureBindingDesc> texture_bindings_;
};

class VertexShader : public ShaderBase
{
public:
    VertexShader(GraphicsContext* context, const std::string& asset_path);
    virtual ~VertexShader() {};

    virtual void Create(GraphicsContext& context) override;
    virtual void Bind(GraphicsContext& context) override;

protected:
    virtual void Reflect(GraphicsContext& context) override;

    void CreateInputLayoutFromReflection(const GraphicsContext& context);

    ComPtr<ID3D11VertexShader> native_ptr_;
    ComPtr<ID3D11InputLayout> input_layout_;
};

class PixelShader : public ShaderBase
{
public:
    PixelShader(GraphicsContext* context, const std::string& asset_path);
    virtual ~PixelShader() {};

    virtual void Create(GraphicsContext& context) override;
    virtual void Bind(GraphicsContext& context) override;

protected:
    ComPtr<ID3D11PixelShader> native_ptr_;
};
