#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

SharedPtr<MeshData> MeshData::LoadFromFile(const GraphicsContext& context, const std::string& asset_path)
{
    LOG("Loading mesh: {}", asset_path);

    SharedPtr<MeshData> mesh_data = MakeShared<MeshData>();

    std::vector<uint16> indices;
    std::vector<Vec3> pos;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;

    Assimp::Importer importer;

    std::vector<char> bytes = FileIO::ReadFile(asset_path);

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

    mesh_data->pos = MakeShared<VertexBuffer>(context, pos.data(), (uint32)pos.size(), sizeof(Vec3), VertexBufferSlots::POS);
    mesh_data->normals = MakeShared<VertexBuffer>(context, normals.data(), (uint32)normals.size(), sizeof(Vec3), VertexBufferSlots::NORMALS);
    mesh_data->uv = MakeShared<VertexBuffer>(context, uvs.data(), (uint32)uvs.size(), sizeof(Vec2), VertexBufferSlots::TEX_COORD);
    mesh_data->index_buffer = MakeShared<IndexBuffer>(context, indices.data(), (uint32)indices.size());
    return mesh_data;
}

void MeshData::Bind(GraphicsContext& context)
{
    index_buffer->Bind(context);
    pos->Bind(context);
    uv->Bind(context);
    normals->Bind(context);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    LOG("Destroy mesh");
}

void Mesh::Render(GraphicsContext& context)
{
    CHECK(mesh_data_ != nullptr);
    CHECK(material_ != nullptr);

    Bind(context);
    context.device_context->DrawIndexed(mesh_data_->index_buffer->GetNum(), 0 /*start idx*/, 0 /*idx offset*/);
}

void Mesh::Bind(GraphicsContext& context)
{
    context.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CHECK(material_!= nullptr);
    material_->Bind(context);

    CHECK(mesh_data_ != nullptr);
    mesh_data_->Bind(context);

    if(cbuffer_per_object_ == nullptr)
    {
        D3D11_BUFFER_DESC cbuffer_desc = {};
        cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
        cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbuffer_desc.ByteWidth = sizeof(CBufferPerObject);
        cbuffer_desc.CPUAccessFlags = 0;
        DX11_VERIFY(context.device->CreateBuffer(&cbuffer_desc, nullptr, &cbuffer_per_object_));
    }

    context.device_context->UpdateSubresource(cbuffer_per_object_.Get(), 0, nullptr, &per_object_data_, 0, 0);

    static constexpr int CBUFFER_PER_OBJECT_SLOT = 1;
    context.device_context->VSSetConstantBuffers(CBUFFER_PER_OBJECT_SLOT, 1, cbuffer_per_object_.GetAddressOf());
    context.device_context->PSSetConstantBuffers(CBUFFER_PER_OBJECT_SLOT, 1, cbuffer_per_object_.GetAddressOf());
}

void Mesh::Update(float dt)
{
    transform_.rotation_ *= Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(25.0f) * dt);
    transform_.rotation_.Normalize();
    per_object_data_.mat_world = transform_.ToMatrix().Transpose();
}
