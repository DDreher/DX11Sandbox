#include "AppLighting.h"

#include "backends/imgui_impl_sdl.h"
#include "SDL_events.h"
#include "spdlog/fmt/bundled/ostream.h"

#include "Engine/Entity.h"
#include "Engine/SceneImporter.h"

#include "Renderer/Renderer.h"

AppLighting::AppLighting()
{
    application_name_ = "Lighting";
}

void AppLighting::Init()
{
    BaseApplication::Init();

    {
        SharedPtr<Entity> entity = MakeShared<Entity>();
        entity->name_ = "Directional Light";
        DirectionalLightComponent* directional_light = entity->AddComponent<DirectionalLightComponent>();
        entity->transform_->SetWorldRotation(Quat::FromAxisAngle(Vec3::RIGHT, 45.0f));
        world.Add(entity);
    }

    {
        String model_path = "assets/models/sponza/Sponza.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldRotation(Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(90.0f)));
        import_correction_transform.SetWorldTranslation({ 0.0f, 0.0f, 0.0f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };
        SceneImporter::ImportScene(scene_desc, world);
    }

    {
        String model_path = "assets/meshes/sphere.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldTranslation({ 0.0f, 1.0f, -3.0f });
        import_correction_transform.SetWorldScaling({ 0.33f, 0.33f, 0.33f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };
        SharedPtr<Entity> sphere = SceneImporter::ImportScene(scene_desc, world);
        sphere->name_ = "Point Light";
        PointLightComponent* light = sphere->AddComponent<PointLightComponent>();
        light->color_ = { 0.0f, 0.0f, 1.0f };
    }

    {
        String model_path = "assets/meshes/sphere.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldTranslation({ 0.0f, 1.0f, -5.0f });
        import_correction_transform.SetWorldScaling({ 0.33f, 0.33f, 0.33f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };
        SharedPtr<Entity> sphere = SceneImporter::ImportScene(scene_desc, world);
        sphere->name_ = "Spot Light";
        SpotLightComponent* light = sphere->AddComponent<SpotLightComponent>();
        light->color_ = { 1.0f, 0.0f, 0.0f };
        light->cone_angle_ = 33.33f;
    }

    {
        String model_path = "assets/meshes/sphere.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldTranslation({ 0.0f, 1.0f, -5.0f });
        import_correction_transform.SetWorldScaling({ 0.33f, 0.33f, 0.33f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };
        SharedPtr<Entity> sphere = SceneImporter::ImportScene(scene_desc, world);
        sphere->name_ = "Spot Light 2";
        SpotLightComponent* light = sphere->AddComponent<SpotLightComponent>();
        light->color_ = { 1.0f, 0.0f, 0.0f };
        light->cone_angle_ = 33.33f;
    }
}

void AppLighting::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppLighting::Update()
{
    BaseApplication::Update();
}

void AppLighting::Render()
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
                item.mesh = &mesh;
                item.sort_key = Vec3::DistanceSquared(gfx::camera.GetPosition(), mesh_component->model_->transform.GetWorldTranslation());
                // Note: For now we calculate the distance from camera to entity... This is not ideal, especially for the render order of meshes w/ transparent materials.
                // Problems for future me, I guess :>

                Material* material = gfx::resource_manager->materials.Get(mesh_component->model_->materials_[mesh.material_slot]);
                CHECK(material != nullptr);
                gfx::renderer->Enqueue(item, material->blend_state_);
            }
        }

        {
            DirectionalLightComponent* light = entity->GetComponent<DirectionalLightComponent>();
            if (light != nullptr && light->is_enabled_)
            {
                Renderer* renderer = (Renderer*) gfx::renderer;
                renderer->Enqueue(DirectionalLight(entity->transform_->GetWorldForward().xyz().Normalize(), light->ambient_intensity_,
                    light->color_, light->brightness_));
            }
        }

        {
            PointLightComponent* light = entity->GetComponent<PointLightComponent>();
            if (light != nullptr && light->is_enabled_)
            {
                Renderer* renderer = (Renderer*) gfx::renderer;
                renderer->Enqueue(PointLight{ entity->transform_->GetWorldTranslation(), light->ambient_intensity_,
                    light->color_, light->attenuation_, light->brightness_ });
            }
        }

        {
            SpotLightComponent* light = entity->GetComponent<SpotLightComponent>();
            if (light != nullptr && light->is_enabled_)
            {
                Renderer* renderer = (Renderer*)gfx::renderer;
                renderer->Enqueue(SpotLight
                    {
                        .position_ws = entity->transform_->GetWorldTranslation(),
                        .ambient_intensity = light->ambient_intensity_,
                        .color = light->color_,
                        .attenuation = light->attenuation_,
                        .cone_dir_ws = entity->transform_->GetWorldForward().xyz().Normalize(),
                        .cos_cone_angle =  cosf(MathUtils::DegToRad(light->cone_angle_)),
                        .brightness = light->brightness_
                    });
            }
        }
    }

    BaseApplication::Render();
}

void AppLighting::RenderUI()
{
    BaseApplication::RenderUI();

    ImGui::Begin("Debug Menu");

    for (SharedPtr<Entity> e : world.GetEntities())
    {
        if(DirectionalLightComponent* l = e->GetComponent<DirectionalLightComponent>())
        {
            ImGui::Text("%s", e->name_.c_str());
            ImGui::Checkbox("enabled", &l->is_enabled_);
            //ImGui::SliderFloat3("LightDir", reinterpret_cast<float*>(&e->transform_->), -10.0f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("Brightness##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->brightness_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::ColorPicker3("LightColor", reinterpret_cast<float*>(&l->color_));
            ImGui::SliderFloat("AmbientIntensity", reinterpret_cast<float*>(&l->ambient_intensity_), 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }

        if (PointLightComponent* l = e->GetComponent<PointLightComponent>())
        {
            ImGui::Text("%s", e->name_.c_str());
            ImGui::Checkbox(fmt::format("is_enabled##{}", e->name_).c_str(), &l->is_enabled_);
            Vec3 pos = e->transform_->GetWorldTranslation();
            ImGui::SliderFloat3(fmt::format("Pos##{}", e->name_).c_str(), reinterpret_cast<float*>(&pos), -15.0f, 15.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            e->transform_->SetWorldTranslation(pos);
            ImGui::SliderFloat(fmt::format("Brightness##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->brightness_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::ColorPicker3(fmt::format("Color##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->color_));
            ImGui::SliderFloat(fmt::format("AmbientIntensity##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->ambient_intensity_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("Attenuation##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->attenuation_),
                0.0f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }

        if (SpotLightComponent* l = e->GetComponent<SpotLightComponent>())
        {
            ImGui::Text("%s", e->name_.c_str());
            ImGui::Checkbox(fmt::format("is_enabled##{}", e->name_).c_str(), &l->is_enabled_);

            Vec3 pos = e->transform_->GetWorldTranslation();
            ImGui::SliderFloat3(fmt::format("Pos##{}", e->name_).c_str(), reinterpret_cast<float*>(&pos), -15.0f, 15.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            e->transform_->SetWorldTranslation(pos);

            Vec3 world_rot = e->transform_->GetWorldRotation().ToEuler();
            ImGui::SliderFloat3(fmt::format("Rot##{}", e->name_).c_str(), reinterpret_cast<float*>(&world_rot), -1.0, 1.0f,
                "%.3f", ImGuiSliderFlags_AlwaysClamp);
            e->transform_->SetWorldRotation(Quat::Normalize(Quat::FromPitchYawRoll(world_rot)));
            
            ImGui::SliderFloat(fmt::format("Brightness##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->brightness_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("ConeAngle##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->cone_angle_),
                0.01f, 90.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::ColorPicker3(fmt::format("Color##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->color_));
            ImGui::SliderFloat(fmt::format("AmbientIntensity##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->ambient_intensity_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("Attenuation##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->attenuation_),
                0.0f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }
    }

    ImGui::End();
}

void AppLighting::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
