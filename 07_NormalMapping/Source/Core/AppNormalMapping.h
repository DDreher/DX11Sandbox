#pragma once
#include "Core/Application.h"

class AppNormalMapping : public BaseApplication
{
public:
    AppNormalMapping();

protected:
    virtual void Init() final;
    virtual void Cleanup() final;
    virtual void Update() final;
    virtual void Render() final;
    virtual void RenderUI() final;

    void HandleSDLEvent(const SDL_Event& sdl_event) final;
};
