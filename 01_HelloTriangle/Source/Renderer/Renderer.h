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
#include "Renderer/DX11Types.h"

using namespace DirectX;

struct VertexShader
{
    void LoadFromHlsl(ID3D11Device* const device, const std::string& path);

    ComPtr<ID3D11VertexShader> vs = nullptr;
    ComPtr<ID3D11InputLayout> input_layout = nullptr; // Describes order and type of input data
};

struct PixelShader
{
    void LoadFromHlsl(ID3D11Device* const device, const std::string& path);
    ComPtr<ID3D11PixelShader> ps = nullptr;
};

struct VertexPos
{
    VertexPos() {}
    VertexPos(Vec3 in_pos)
        : pos(in_pos)
    {
    }

    VertexPos(float x, float y, float z)
        : pos(x, y, z)
    {
    }

    Vec3 pos;

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] = 
    {
        { 
            "POSITION", // Just a name for the layout element. Used for mapping in the VS
            0,          // Additional index for mapping. Here we'll actually have POSITION0
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,    // Format of the element -> this will be a vec3f
            0,  // input slot index, range 0-15.
            0,  // Memory offset (bytes). Position is at the start of the memory block -> 0.
                // If we add color in same input slot, its offset would be sizeof(vec3f) = 12 bytes
            D3D11_INPUT_PER_VERTEX_DATA,    // InputSlotClass - Used for instancing (?)
            0   // InstanceDataStepRate - Also used for instancing (?)
        },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};


struct VertexPosColor
{
    Vec3 pos_;
    Vec3 color_;

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
    {
        { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};

class Renderer
{
public:
    Renderer(Window* window);
    Renderer(const Renderer&) = delete;                 // <-- No copy!
    Renderer& operator=(const Renderer&) = delete;      // <-/
    ~Renderer() = default;

    void Render();

private:
    template<typename ShaderType>
    ShaderType* CreateShader(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage);

    template<>
    ID3D11VertexShader* CreateShader<ID3D11VertexShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
    {
        CHECK(device_ != nullptr);
        CHECK(shader_blob != nullptr);
        ID3D11VertexShader* shader = nullptr;
        device_->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), class_linkage, &shader);

        CHECK(shader != nullptr);
        return shader;
    }

    template<>
    ID3D11PixelShader* CreateShader<ID3D11PixelShader>(ID3DBlob* shader_blob, ID3D11ClassLinkage* class_linkage)
    {
        CHECK(device_ != nullptr);
        CHECK(shader_blob != nullptr);
        ID3D11PixelShader* shader = nullptr;
        device_->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), class_linkage, &shader);

        CHECK(shader != nullptr);
        return shader;
    }

    ID3D11VertexShader* CreateVertexShader(const std::string& path);
    ID3D11PixelShader* CreatePixelShader(const std::string& path);

    ComPtr<ID3D11Device> device_ = nullptr;                // Used for everything besides rendering :P Useful for multithreading
    ComPtr<ID3D11DeviceContext> device_context_ = nullptr; // Used to configure the rendering pipeline and draw geometry

    ComPtr<IDXGISwapChain> swapchain_ = nullptr;
    ComPtr<ID3D11RenderTargetView> backbuffer_color_view_ = nullptr;   // Views for "output" of the swapchain

    // Pipeline configuration
    D3D11_VIEWPORT viewport_ = { 0 };   // Defines renderable area on screen

    // Mesh Buffers
    ComPtr<ID3D11Buffer> vertex_buffer_ = nullptr;
    ComPtr<ID3D11Buffer> index_buffer_ = nullptr;

    // Shaders
    VertexShader vertex_shader_;
    PixelShader pixel_shader_;

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
