#pragma once
#include "Core/Application.h"
#include "Renderer/Renderer.h"

class AppModels : public BaseApplication
{
public:
    AppModels();

protected:
    void Init() override;
    void Cleanup() override;
    void Update(double dt) override;
    void Render() override;

    void HandleSDLEvent(const SDL_Event& sdl_event) override;

};
