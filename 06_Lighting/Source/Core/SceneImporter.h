#pragma once
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Renderer/Mesh.h"

struct SceneDescription
{
    String path;
    Transform import_correction_transform;
};

struct aiScene;
struct aiNode;

class SceneImporter
{
public:
    static SharedPtr<Entity> ImportScene(const SceneDescription& scene_desc, World& world);

private:

    static SharedPtr<Entity> ProcessNode(const SceneDescription& scene_desc, const aiScene* scene, aiNode* node, Entity* parent, World& world);
    static SharedPtr<Model> ProcessMesh(const SceneDescription& scene_desc, const aiScene* scene, aiNode* node, uint32 mesh_idx);
};
