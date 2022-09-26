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
}

void AppShaderReflection::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppShaderReflection::Update()
{
    BaseApplication::Update();
}

void AppShaderReflection::Render()
{
    BaseApplication::Render();
}

void AppShaderReflection::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
