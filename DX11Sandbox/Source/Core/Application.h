#pragma once
#include "TickTimer.h"
#include "System.h"
#include "Window.h"

class BaseApplication
{
public:
    void Run();

    void AddSystem(SharedPtr<ISystem>&& system);

protected:
    virtual void Init();
    void MainLoop();
    virtual void Cleanup();

    virtual void Update(double dt);
    virtual void Render();

    void InitWindow();
    void DestroyWindow();

    std::vector<SharedPtr<ISystem>> systems_;

    std::string application_name_;
    Window* window_ = nullptr;
    TickTimer tick_timer_;
};
