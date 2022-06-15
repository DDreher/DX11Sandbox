#include "Core/Application.h"

#include "SDL.h"

#include "Engine/Input.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IRenderer.h"

void BaseApplication::Run()
{
    Init();
    MainLoop();
    Cleanup();
}

void BaseApplication::Init()
{
    LOG("Initializing application: {}", application_name_);

    SDL_Init(SDL_INIT_VIDEO);
    InitWindow();

    gfx::Init(window_);
}

void BaseApplication::MainLoop()
{
    LOG("Entering Main Loop...");

    while (window_->GetIsClosed() == false)
    {
        tick_timer_.Update();
        while (tick_timer_.accumulated_time_ > 0.0)
        {
            double delta_time = std::min(tick_timer_.accumulated_time_, TickTimer::TICK_TIME);
            Update(delta_time);
            tick_timer_.accumulated_time_ -= delta_time;
        }

        Render();
    }
}

void BaseApplication::Cleanup()
{
    LOG("Tearing down application...");
    gfx::Shutdown();
    DestroyWindow();
    SDL_Quit();
}

void BaseApplication::InitWindow()
{
    window_ = new Window(application_name_, 1024, 768);
    CHECK(window_ != nullptr);
}

void BaseApplication::DestroyWindow()
{
    if (window_ != nullptr)
    {
        delete window_;
        window_ = nullptr;
    }
}

void BaseApplication::Update(double dt)
{
    input::ResetMousePosDelta();    // Have to manually reset, otherwise we only update on mouse moved event.

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        HandleSDLEvent(sdl_event);
    }
}

void BaseApplication::HandleSDLEvent(const SDL_Event& sdl_event)
{
    input::HandleSDLEvent(sdl_event);

    if(window_ != nullptr)
    {
        window_->HandleSDLEvent(sdl_event);
    }
}

void BaseApplication::Render()
{
    gfx::renderer->Render();
}
