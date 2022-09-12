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

void AppHelloTriangle::Update(double dt)
{
    BaseApplication::Update(dt);
}

void AppHelloTriangle::Render()
{
    BaseApplication::Render();
}
