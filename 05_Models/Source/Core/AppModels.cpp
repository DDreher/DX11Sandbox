#include "AppModels.h"

#include "backends/imgui_impl_sdl.h"
#include "SDL_events.h"

AppModels::AppModels()
{
    application_name_ = "Models";
}

void AppModels::Init()
{
    BaseApplication::Init();
    gfx::Init(window_);
}

void AppModels::Cleanup()
{
    gfx::Shutdown();
    BaseApplication::Cleanup();
}

void AppModels::Update(double dt)
{
    BaseApplication::Update(dt);
}

void AppModels::Render()
{
    BaseApplication::Render();
    gfx::renderer->Render();
}

void AppModels::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
