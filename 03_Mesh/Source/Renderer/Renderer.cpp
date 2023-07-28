#include "Renderer.h"

#include <d3dcompiler.h>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "Core/Application.h"
#include "Core/FileIO.h"
#include "Renderer/DX11Util.h"
#include "Renderer/Texture.h"
#include "Renderer/Vertex.h"

SharedPtr<MeshData> MeshData::LoadFromFile(const std::string& asset_path)
{
    LOG("Loading mesh: {}", asset_path);

    SharedPtr<MeshData> mesh_data = MakeShared<MeshData>();

    std::vector<uint16> indices;
    std::vector<Vec3> pos;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;

    Assimp::Importer importer;

    std::vector<uint8> bytes = FileIO::ReadFile(asset_path);

    uint32 importer_flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
    const aiScene* scene = importer.ReadFileFromMemory(bytes.data(), bytes.size(), importer_flags);
    CHECK_MSG(scene != nullptr, "Failed to load mesh from file: {}", asset_path);

    for (uint32 mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx)
    {
        aiMesh* mesh = scene->mMeshes[mesh_idx];
        for (uint32 vertex_id = 0; vertex_id < mesh->mNumVertices; ++vertex_id)
        {
            const aiVector3D& vertex = mesh->mVertices[vertex_id];
            pos.push_back({ vertex.x, vertex.y, vertex.z });

            const aiVector3D& normal = mesh->HasNormals() ? mesh->mNormals[vertex_id] : aiVector3D(0.0f, 0.0f, 0.0f);
            normals.push_back({ normal.x, normal.y, normal.z });

            const aiVector3D& uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vertex_id] : aiVector3D(0.0f, 0.0f, 0.0f);
            uvs.push_back({ uv.x, uv.y });
        }

        for (uint32 i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            CHECK(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
    }

    mesh_data->pos = MakeShared<VertexBuffer>(pos.data(), (uint32)pos.size(), sizeof(Vec3), VertexBufferSlots::POS);
    mesh_data->normals = MakeShared<VertexBuffer>(normals.data(), (uint32)normals.size(), sizeof(Vec3), VertexBufferSlots::NORMALS);
    mesh_data->uv = MakeShared<VertexBuffer>(uvs.data(), (uint32)uvs.size(), sizeof(Vec2), VertexBufferSlots::TEX_COORD);
    mesh_data->index_buffer = MakeShared<IndexBuffer>(indices.data(), (uint32)indices.size());
    return mesh_data;
}

void MeshData::Bind()
{
    index_buffer->Bind();
    pos->Bind();
    uv->Bind();
    normals->Bind();
}

void Mesh::Update(float dt)
{
    Quat rot = transform_.GetWorldRotation() * Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(.5f) * dt);
    rot.Normalize();
    transform_.SetWorldRotation(rot);
    per_object_data_.mat_world = transform_.GetWorldMatrix().Transpose();
}

void Mesh::Render()
{
    CHECK(mesh_data_ != nullptr);
    CHECK(material_ != nullptr);

    if (material_->texture_parameters_["tex"].tex.IsValid())
    {
        Bind();
        gfx::device_context->DrawIndexed(mesh_data_->index_buffer->GetNum(), 0 /*start idx*/, 0 /*idx offset*/);
    }
}

void Mesh::Bind()
{
    gfx::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CHECK(material_ != nullptr);
    material_->Bind();

    CHECK(mesh_data_ != nullptr);
    mesh_data_->Bind();

    if (cbuffer_per_object_ == nullptr)
    {
        D3D11_BUFFER_DESC cbuffer_desc = {};
        cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
        cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbuffer_desc.ByteWidth = sizeof(CBufferPerObject);
        cbuffer_desc.CPUAccessFlags = 0;
        DX11_VERIFY(gfx::device->CreateBuffer(&cbuffer_desc, nullptr, &cbuffer_per_object_));
    }

    gfx::device_context->UpdateSubresource(cbuffer_per_object_.Get(), 0, nullptr, &per_object_data_, 0, 0);

    static constexpr int CBUFFER_PER_OBJECT_SLOT = 2;
    gfx::device_context->VSSetConstantBuffers(CBUFFER_PER_OBJECT_SLOT, 1, cbuffer_per_object_.GetAddressOf());
    gfx::device_context->PSSetConstantBuffers(CBUFFER_PER_OBJECT_SLOT, 1, cbuffer_per_object_.GetAddressOf());
}

Renderer::Renderer()
{
    // Get render target view from swapchain backbuffer
    // Even with triple buffering we only need a single render target view
    ComPtr<ID3D11Texture2D> backbuffer;
    DX11_VERIFY(gfx::swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer));

    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
    render_target_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    DX11_VERIFY(gfx::device->CreateRenderTargetView(
        backbuffer.Get(), // Ptr to render target
        &render_target_view_desc,    // Ptr to D3D11_RENDER_TARGET_VIEW_DESC, nullptr to create view of entire subresource at mipmap lvl 0
        &backbuffer_color_view_));

    // Create depth/stencil buffer and view
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    DX11_VERIFY(gfx::swapchain->GetDesc1(&swap_chain_desc));

    D3D11_TEXTURE2D_DESC depth_buffer_desc = {};
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0; // No CPU access
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.Width = swap_chain_desc.Width;
    depth_buffer_desc.Height = swap_chain_desc.Height;
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.SampleDesc.Count = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;  // Read and write access by the GPU

    DX11_VERIFY(gfx::device->CreateTexture2D(&depth_buffer_desc,
        nullptr,    // ptr to initial data
        &depth_buffer_));

    DX11_VERIFY(gfx::device->CreateDepthStencilView(depth_buffer_.Get(), nullptr, &backbuffer_depth_view_));

    // Configure viewport, i.e. the renderable area
    viewport_.Width = (float) swap_chain_desc.Width;
    viewport_.Height = (float) swap_chain_desc.Height;
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    gfx::device_context->RSSetViewports(1, &viewport_);

    // Bind default global render states
    gfx::device_context->VSSetSamplers(0, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointClamp).GetAddressOf());
    gfx::device_context->VSSetSamplers(1, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointWrap).GetAddressOf());
    gfx::device_context->VSSetSamplers(2, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearClamp).GetAddressOf());
    gfx::device_context->VSSetSamplers(3, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearWrap).GetAddressOf());
    gfx::device_context->PSSetSamplers(0, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointClamp).GetAddressOf());
    gfx::device_context->PSSetSamplers(1, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointWrap).GetAddressOf());
    gfx::device_context->PSSetSamplers(2, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearClamp).GetAddressOf());
    gfx::device_context->PSSetSamplers(3, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearWrap).GetAddressOf());

    mesh_.mesh_data_ = MeshData::LoadFromFile("assets/meshes/BOSS_model_final.fbx");
    Handle<Texture> tex = gfx::resource_manager->textures.Create({ .file_path = "assets/textures/BOSS_texture_final.png" });

    MaterialDesc material_unlit_textured_desc
    {
        .vs_path = "assets/shaders/forward_unlit_vs.hlsl",
        .ps_path = "assets/shaders/forward_unlit_ps.hlsl",
        .rasterizer_state = RasterizerState::CullCounterClockwise,
        .blend_state = BlendState::Opaque,
        .depth_stencil_state = DepthStencilState::Default
    };

    SharedPtr<Material> material_unlit_textured = MakeShared<Material>(material_unlit_textured_desc);
    material_unlit_textured->SetTexture("tex", tex);
    material_unlit_textured->SetParam("tint", { 1.0f, 0.0f, 1.0f });

    mesh_.material_ = material_unlit_textured;
    mesh_.transform_.SetWorldTranslation({ 0.0f, -1.0f, 0.0f });

    // Set up cbuffer
    cbuffer_per_view_ = MakeUnique<ConstantBuffer>(sizeof(CBufferPerView));

    // Set up camera
    float aspect_ratio = (float)swap_chain_desc.Width / (float)swap_chain_desc.Height;
    gfx::camera = Camera(Vec3(0.0f, 5.0f, -10.0f), aspect_ratio, MathUtils::DegToRad(45.0f), .1f, 1000.0f);
    gfx::camera.LookAt(Vec3(0.0f, 0.0f, 0.0f) + Vec3(0.0f, 2.0f, 0.0f));
}

Renderer::~Renderer()
{
}

void Renderer::Render()
{
    // Update objects (temporarily in here for testing purposes)
    static float dt = BaseApplication::Get()->GetTimestep();
    mesh_.Update(dt);

    // Bind render target views to output merger stage of pipeline
    gfx::device_context->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), backbuffer_depth_view_.Get());

    // -------------------------------------------------------------------------------
    // Clear backbuffer
    gfx::device_context->ClearRenderTargetView(backbuffer_color_view_.Get(), clear_color_);
    gfx::device_context->ClearDepthStencilView(backbuffer_depth_view_.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

    // -------------------------------------------------------------------------------
    // Render scene

    // Update per frame cbuffer
    per_view_data_.mat_view_projection = gfx::camera.GetViewProjection().Transpose();   // CPU: row major, GPU: col major! -> We have to transpose.

    cbuffer_per_view_->Upload(reinterpret_cast<uint8*>(&per_view_data_), sizeof(per_view_data_));
    static constexpr int SLOT_PER_VIEW = 1;
    gfx::device_context->VSSetConstantBuffers(SLOT_PER_VIEW, 1, cbuffer_per_view_->buffer_.GetAddressOf());
    gfx::device_context->PSSetConstantBuffers(SLOT_PER_VIEW, 1, cbuffer_per_view_->buffer_.GetAddressOf());

    // Submit draw commands
    mesh_.Render();
}

IRenderer* CreateRenderer()
{
    return new Renderer();
}
