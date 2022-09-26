#include "Core/AppModels.h"

#include "backends/imgui_impl_sdl.h"
#include "SDL_events.h"

#include "Core/ModelImporter.h"

#include "Engine/Entity.h"

AppModels::AppModels()
{
    application_name_ = "Models";
}

void AppModels::Init()
{
    BaseApplication::Init();

    String model_path = "assets/models/sponza/Sponza.gltf";
    Transform import_correction_transform;
    import_correction_transform.SetWorldRotation(Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(90.0f)));
    import_correction_transform.SetWorldTranslation({ 0.0f, 0.0f, 0.0f });

    ModelDesc model_desc
    {
        .path = model_path,
        .import_correction_transform = import_correction_transform
    };
    ModelImporter::ImportModel(model_desc, world);
}

void AppModels::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppModels::Update()
{
    BaseApplication::Update();
    world.Update();
}

void AppModels::Render()
{
    for (auto entity : world.GetEntities())
    {
        StaticMeshComponent* mesh_component = entity->GetComponent<StaticMeshComponent>();
        if(mesh_component != nullptr && mesh_component->model_ != nullptr)
        {
            mesh_component->model_->transform = *entity->transform_;
            for(StaticMesh& mesh : mesh_component->model_->meshes_)
            {
                Material* material = gfx::resource_manager->materials.Get(mesh_component->model_->materials_[mesh.material_slot]);
                CHECK(material != nullptr);

                RenderWorkItem item;
                item.mesh = &mesh;
                // TODO: Calculate a sort key in order to minimize state changes
                // https://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
                item.sort_key = Vec3::DistanceSquared(gfx::camera.GetPosition(), mesh_component->model_->transform.GetWorldTranslation());

                gfx::renderer->Enqueue(item, material->blend_state_);
            }
        }
    }

    BaseApplication::Render();
}

void AppModels::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
