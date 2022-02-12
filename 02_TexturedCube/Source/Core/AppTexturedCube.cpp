#include "AppTexturedCube.h"

AppTexturedCube::AppTexturedCube()
{
    application_name_ = "Textured Cube";
}

void AppTexturedCube::Init()
{
    BaseApplication::Init();
    renderer_ = MakeUnique<Renderer>(window_);
}

void AppTexturedCube::Cleanup()
{
    renderer_.release();
    BaseApplication::Cleanup();
}

void AppTexturedCube::Update(double dt)
{
}

void AppTexturedCube::Render()
{
    BaseApplication::Render();
    renderer_->Render();
}
