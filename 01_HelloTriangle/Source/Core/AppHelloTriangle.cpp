#include "AppHelloTriangle.h"

AppHelloTriangle::AppHelloTriangle()
{
    application_name_ = "Hello Triangle!";
}

void AppHelloTriangle::Init()
{
    BaseApplication::Init();
    renderer_ = MakeUnique<Renderer>(window_);
}

void AppHelloTriangle::Cleanup()
{
    renderer_.reset();
    BaseApplication::Cleanup();
}

void AppHelloTriangle::Update(double dt)
{
    BaseApplication::Update(dt);
}

void AppHelloTriangle::Render()
{
    BaseApplication::Render();
    renderer_->Render();
}
