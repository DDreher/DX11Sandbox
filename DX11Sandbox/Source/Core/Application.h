#pragma once
#include "TickTimer.h"
#include "Window.h"

class BaseApplication
{
public:
    void Run();

protected:
    virtual void Init();
    void MainLoop();
    virtual void Cleanup();

    virtual void Update(double dt);
    virtual void HandleSDLEvent(const SDL_Event& sdl_event);
    virtual void Render();

    void InitWindow();
    void DestroyWindow();

    std::string application_name_;
    Window* window_ = nullptr;
    TickTimer tick_timer_;
};
