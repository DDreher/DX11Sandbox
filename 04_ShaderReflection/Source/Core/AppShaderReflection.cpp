#include "AppShaderReflection.h"

#include "backends/imgui_impl_sdl.h"
#include "SDL_events.h"

AppShaderReflection::AppShaderReflection()
{
    application_name_ = "Shader Reflection";
}

void AppShaderReflection::Init()
{
    BaseApplication::Init();
    renderer_ = MakeUnique<Renderer>(window_);
}

void AppShaderReflection::Cleanup()
{
    renderer_.reset();

    BaseApplication::Cleanup();
}

void AppShaderReflection::Update(double dt)
{
    BaseApplication::Update(dt);
}

void AppShaderReflection::Render()
{
    BaseApplication::Render();
    renderer_->Render();
}

void AppShaderReflection::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
