#pragma once
#include "Core/Application.h"
#include "Engine/World.h"
#include "Renderer/Renderer.h"

class AppModels : public BaseApplication
{
public:
    AppModels();

protected:
    virtual void Init() final;
    virtual void Cleanup() final;
    virtual void Update(double dt) final;
    virtual void Render() final;

    void HandleSDLEvent(const SDL_Event& sdl_event) final;

    World world;
};
