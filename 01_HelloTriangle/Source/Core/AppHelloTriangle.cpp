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
    // No update required, it's just a static triangle :)
}

void AppHelloTriangle::Render()
{
    BaseApplication::Render();
}
