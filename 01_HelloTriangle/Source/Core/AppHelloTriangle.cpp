#include "AppHelloTriangle.h"

AppHelloTriangle::AppHelloTriangle()
{
    application_name_ = "Hello Triangle!";
}

void AppHelloTriangle::Init()
{
    BaseApplication::Init();
}

void AppHelloTriangle::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppHelloTriangle::Update()
{
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        HandleSDLEvent(sdl_event);
    }
}

void AppHelloTriangle::Render()
{
    BaseApplication::Render();
}
