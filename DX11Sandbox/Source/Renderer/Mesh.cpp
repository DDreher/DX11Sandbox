#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "Renderer/GraphicsContext.h"

void StaticMesh::Bind() const
{
    index_buffer->Bind();
    pos->Bind();
    normals->Bind();
    tangents->Bind();
    uv->Bind();

    // TODO: ... Why do I store the materials in the model again?
    Material* material = gfx::resource_manager->materials.Get(model->materials_[material_slot]);
    material->Bind();
}

void StaticMesh::Render() const
{
    gfx::device_context->DrawIndexed(num_indices, start_idx, offset);
}

SharedPtr<Model> MeshImporter::LoadFromFile(const MeshFileDesc& desc)
{
    LOG("Loading mesh: {}", desc.path);
    auto root_path = std::filesystem::path(desc.path).parent_path();

    SharedPtr<Model> model = MakeShared<Model>();

    Assimp::Importer ai_importer;
    uint32 importer_flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;
    const aiScene* ai_scene = ai_importer.ReadFile(desc.path, importer_flags);
    CHECK_MSG(ai_scene != nullptr, "Failed to load mesh from file: {}. \n Error: {}", desc.path, ai_importer.GetErrorString());

    model->transform = desc.correction_transform;

    for (uint32 i = 0; i < ai_scene->mNumMaterials; ++i)
    {
        aiMaterial* ai_material = ai_scene->mMaterials[i];

        BlendState material_blendstate = BlendState::Opaque;
        aiBlendMode blend_mode;
        if (ai_material->Get(AI_MATKEY_BLEND_FUNC, blend_mode) == AI_SUCCESS)
        {
            switch (blend_mode)
            {
            case aiBlendMode::aiBlendMode_Default:
                material_blendstate = BlendState::Opaque;
                break;
            case aiBlendMode::aiBlendMode_Additive:
                material_blendstate = BlendState::Additive;
                break;
            default:
                CHECK_NO_ENTRY();
            }
        }
        else // aiBlendMode is not always set, so we also have to check for material opacity...
        {
            float mat_opacity;
            if (ai_material->Get(AI_MATKEY_OPACITY, mat_opacity) == AI_SUCCESS)
            {
                material_blendstate = mat_opacity < 1.0f ? BlendState::NonPremultipliedAlpha : BlendState::Opaque;
            }
        }

        MaterialDesc mat_desc_textured
        {
            .vs_path = "assets/shaders/forward_phong_vs.hlsl",
            .ps_path = "assets/shaders/forward_phong_ps.hlsl",
            .rasterizer_state = RasterizerState::CullCounterClockwise,
            .blend_state = material_blendstate,
            .depth_stencil_state = DepthStencilState::Default

            // TODO: Do we have to add more stuff here? Texture parameters etc
        };

        Handle<Material> mat_handle = gfx::resource_manager->materials.Create(mat_desc_textured);
        Material* mat = gfx::resource_manager->materials.Get(mat_handle);

        aiString diffuse_tex_path;
        if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_tex_path) == AI_SUCCESS &&
            diffuse_tex_path.C_Str() != "")
        {
            std::filesystem::path tex_path = root_path / std::filesystem::path(diffuse_tex_path.C_Str());
            Handle<Texture> tex = gfx::resource_manager->textures.GetHandle(TextureDesc{ tex_path.string() });

            mat->SetTexture("tex", tex);
        }

        model->materials_.push_back(mat_handle);
    }

    VertexData vertex_data;

    std::deque<aiNode*> nodes;
    nodes.push_back(ai_scene->mRootNode);

    std::vector<int> preorder_indices;

    // Preorder traversal of scene tree
    uint32 num_model_indices = 0;
    uint32 num_model_vertices = 0;
    while(nodes.size() > 0)
    {
        aiNode* ai_node = nodes.back();
        nodes.pop_back();

        aiVector3D scaling;
        aiQuaternion rotation;
        aiVector3D translation;
        ai_node->mTransformation.Decompose(scaling, rotation, translation);

        for (uint32 mesh_idx = 0; mesh_idx < ai_node->mNumMeshes; ++mesh_idx)
        {
            aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[mesh_idx]];

            uint32 num_mesh_indices = 0;
            for (uint32 i = 0; i < ai_mesh->mNumFaces; ++i)
            {
                const aiFace& face = ai_mesh->mFaces[i];
                CHECK(face.mNumIndices == 3);
                vertex_data.indices.push_back(num_model_vertices + face.mIndices[0]);
                vertex_data.indices.push_back(num_model_vertices + face.mIndices[1]);
                vertex_data.indices.push_back(num_model_vertices + face.mIndices[2]);
                num_mesh_indices += 3;
            }

            num_model_indices += num_mesh_indices;

            for (uint32 vertex_id = 0; vertex_id < ai_mesh->mNumVertices; ++vertex_id)
            {
                const aiVector3D& vertex = ai_mesh->mVertices[vertex_id];
                vertex_data.pos.push_back({ vertex.x, vertex.y, vertex.z });

                const aiVector3D& normal = ai_mesh->HasNormals() ? ai_mesh->mNormals[vertex_id] : aiVector3D(0.0f, 0.0f, 0.0f);
                vertex_data.normals.push_back({ normal.x, normal.y, normal.z });

                const aiVector3D& uv = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][vertex_id] : aiVector3D(0.0f, 0.0f, 0.0f);
                vertex_data.uvs.push_back({ uv.x, uv.y });

                num_model_vertices++;
            }

            StaticMesh mesh;

            mesh.start_idx = num_model_indices - num_mesh_indices;
            mesh.offset = 0;
            mesh.num_indices = num_mesh_indices;
            mesh.material_slot = ai_mesh->mMaterialIndex;
            mesh.model = model.get();

            model->meshes_.push_back(mesh);
        }

        for(uint32 i=0; i< ai_node->mNumChildren; ++i)
        {
            nodes.push_back(ai_node->mChildren[i]);
        }
    }

    D3D11_BUFFER_DESC cbuffer_desc = {};
    cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbuffer_desc.ByteWidth = sizeof(CBufferPerObject);
    cbuffer_desc.CPUAccessFlags = 0;
    DX11_VERIFY(gfx::device->CreateBuffer(&cbuffer_desc, nullptr, &model->cbuffer_per_object));

    model->index_buffer = MakeShared<IndexBuffer>(vertex_data.indices.data(), (uint32) vertex_data.indices.size());
    SetDebugName(model->index_buffer->GetNativePtr().Get(), desc.path + " index");
    model->pos = MakeShared<VertexBuffer>(vertex_data.pos.data(), (uint32) vertex_data.pos.size(), sizeof(Vec3), VertexBufferSlots::POS);
    SetDebugName(model->pos->GetNativePtr().Get(), desc.path + " pos");
    model->normals = MakeShared<VertexBuffer>(vertex_data.normals.data(), (uint32) vertex_data.normals.size(), sizeof(Vec3), VertexBufferSlots::NORMALS);
    SetDebugName(model->normals->GetNativePtr().Get(), desc.path + " normals");
    model->uv = MakeShared<VertexBuffer>(vertex_data.uvs.data(), (uint32) vertex_data.uvs.size(), sizeof(Vec2), VertexBufferSlots::TEX_COORD);
    SetDebugName(model->uv->GetNativePtr().Get(), desc.path + " uv0");

    for (StaticMesh& mesh : model->meshes_)
    {
        mesh.index_buffer = model->index_buffer;
        mesh.pos = model->pos;
        mesh.normals = model->normals;
        mesh.uv = model->uv;
    }

    return model;
}

void Model::Bind()
{
    per_object_data.mat_world = transform.GetWorldMatrix().Transpose();
    gfx::device_context->UpdateSubresource(cbuffer_per_object.Get(), 0, nullptr, &per_object_data, 0, 0);

    static constexpr int CBUFFER_SLOT_PER_OBJECT = 2;
    gfx::SetConstantBuffer(cbuffer_per_object.Get(), CBUFFER_SLOT_PER_OBJECT);
}

void Model::Render()
{
    Bind();

    for (size_t i = 0; i < meshes_.size(); ++i)
    {
        auto mesh = meshes_[i];
        Material* material = gfx::resource_manager->materials.Get(materials_[mesh.material_slot]);
        material->Bind();
        mesh.Render();
    }
}
