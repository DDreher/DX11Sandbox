#pragma once
#include "Core/Application.h"
#include "Renderer/Renderer.h"

class AppShaderReflection : public BaseApplication
{
public:
    AppShaderReflection();

protected:
    void Init() override;
    void Cleanup() override;
    void Update() override;
    void Render() override;

    void HandleSDLEvent(const SDL_Event& sdl_event) override;
};
