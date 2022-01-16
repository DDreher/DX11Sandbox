#include "Application.h"

#include "SDL.h"

void BaseApplication::Run()
{
    Init();
    MainLoop();
    Cleanup();
}

void BaseApplication::AddSystem(SharedPtr<ISystem>&& system)
{
    systems_.push_back(system);
}

void BaseApplication::Init()
{
    LOG("Initializing application: {}", application_name_);

    SDL_Init(SDL_INIT_VIDEO);
    InitWindow();
}

void BaseApplication::MainLoop()
{
    LOG("Entering Main Loop...");

    while (window_->GetIsClosed() == false)
    {
        if (window_ != nullptr)
        {
            window_->Update();
        }

        tick_timer_.Update();
        double frame_time = tick_timer_.GetAccumulatedTime();
        while(frame_time > 0.0)
        {
            double delta_time = std::min(frame_time, TickTimer::TICK_TIME);
            Update(delta_time);
            frame_time -= delta_time;
        }

        Render();
    }
}

void BaseApplication::Cleanup()
{
    LOG("Tearing down application...");
    systems_.clear();
    DestroyWindow();
    SDL_Quit();
}

void BaseApplication::InitWindow()
{
    window_ = new Window(application_name_, 800, 600);
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
    for(SharedPtr<ISystem> system : systems_)
    {
        system->Update(dt);
    }
}

void BaseApplication::Render()
{
}
