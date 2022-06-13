#include "AppMesh.h"

#include "Core/AssetLibrary.h"

AppMesh::AppMesh()
{
    application_name_ = "Mesh";
}

void AppMesh::Init()
{
    BaseApplication::Init();
    renderer_ = MakeUnique<Renderer>(window_);
}

void AppMesh::Cleanup()
{
    renderer_.reset();
    BaseApplication::Cleanup();
}

void AppMesh::Update(double dt)
{
    BaseApplication::Update(dt);
}

void AppMesh::Render()
{
    BaseApplication::Render();
    renderer_->Render();
}
