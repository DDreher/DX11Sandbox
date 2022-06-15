#include "AppModels.h"

#include "backends/imgui_impl_sdl.h"
#include "SDL_events.h"

#include "Engine/Entity.h"
#include "Engine/SceneImporter.h"

AppModels::AppModels()
{
    application_name_ = "Models";
}

void AppModels::Init()
{
    BaseApplication::Init();

    String model_path = "assets/models/sponza/Sponza.gltf";
    Transform import_correction_transform;
    import_correction_transform.SetWorldTranslation({ 0.0f, -1.0f, 10.0f });

    SceneDescription scene_desc
    {
        .path = model_path,
        .import_correction_transform = import_correction_transform
    };
    SceneImporter::ImportScene(scene_desc, world);
}

void AppModels::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppModels::Update(double dt)
{
    BaseApplication::Update(dt);
    world.Update(dt);
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
                RenderWorkItem item;
                item.distance_from_camera = Vec3::DistanceSquared(gfx::camera.GetPosition(), mesh_component->model_->transform.GetWorldTranslation());
                item.mesh = &mesh;
                // Note: For now we calculate the distance from camera to entity... This is not ideal, especially for the render order of meshes w/ transparent materials.
                // Problems for future me, I guess :>

                Material* material = gfx::resource_manager->materials.Get(mesh_component->model_->materials_[mesh.material_slot]);
                CHECK(material != nullptr);
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
