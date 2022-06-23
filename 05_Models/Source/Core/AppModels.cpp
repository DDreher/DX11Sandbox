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
}

void AppModels::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppModels::Update(double dt)
{
    BaseApplication::Update(dt);
}

void AppModels::Render()
{
    BaseApplication::Render();
}

void AppModels::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
