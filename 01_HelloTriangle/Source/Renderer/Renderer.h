#pragma once
// Link library dependencies
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IRenderer.h"

using namespace DirectX;

struct SimpleVertexShader
{
    void LoadFromHlsl(ID3D11Device* const device, const std::string& path);

    ComPtr<ID3D11VertexShader> vs = nullptr;
    ComPtr<ID3D11InputLayout> input_layout = nullptr; // Describes order and type of input data
};

struct SimplePixelShader
{
    void LoadFromHlsl(ID3D11Device* const device, const std::string& path);
    ComPtr<ID3D11PixelShader> ps = nullptr;
};

class Renderer : public IRenderer
{
public:
    Renderer();
    Renderer(const Renderer&) = delete;                 // <-- No copy!
    Renderer& operator=(const Renderer&) = delete;      // <-/

    virtual void Render() override;

private:
    ID3D11VertexShader* CreateVertexShader(const std::string& path);
    ID3D11PixelShader* CreatePixelShader(const std::string& path);

    template<typename ShaderType>
    ShaderType* CreateShader(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage);

    template<>
    ID3D11VertexShader* CreateShader<ID3D11VertexShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
    {
        CHECK(gfx::device != nullptr);
        CHECK(shader_blob != nullptr);
        ID3D11VertexShader* shader = nullptr;
        gfx::device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), class_linkage, &shader);

        CHECK(shader != nullptr);
        return shader;
    }

    template<>
    ID3D11PixelShader* CreateShader<ID3D11PixelShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
    {
        CHECK(gfx::device != nullptr);
        CHECK(shader_blob != nullptr);
        ID3D11PixelShader* shader = nullptr;
        gfx::device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), class_linkage, &shader);

        CHECK(shader != nullptr);
        return shader;
    }

    ComPtr<ID3D11RenderTargetView> backbuffer_color_view_ = nullptr;   // Views for "output" of the swapchain

    // Pipeline configuration
    D3D11_VIEWPORT viewport_ = { 0 };   // Defines renderable area on screen

    // Mesh Buffers
    ComPtr<ID3D11Buffer> vertex_buffer_ = nullptr;
    ComPtr<ID3D11Buffer> index_buffer_ = nullptr;

    // Shaders
    SimpleVertexShader vertex_shader_;
    SimplePixelShader pixel_shader_;

    // Shader Resources
    ComPtr<ID3D11Buffer> cbuffer_per_object_ = nullptr; // AKA Uniform Buffers in OpenGL land

    // General render settings
    float clear_color_[4] = { 100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 255.0f/255.0f };

    // Mesh data
    VertexPosColor triangle_vertices_[3] =
    {
        { Vec3(0.0f, 0.5f, 0.5f),   Vec3(1.0f, 0.0f, 0.0f) },
        { Vec3(0.5f, -0.5f, 0.5f),  Vec3(0.0f, 1.0f, 0.0f) },
        { Vec3(-0.5f, -0.5f, 0.5f), Vec3(1.0f, 0.0f, 1.0f) }
    };

    uint16 triangle_indices_[3] =
    {
        0, 1, 2,
    };
};

IRenderer* CreateRenderer();
