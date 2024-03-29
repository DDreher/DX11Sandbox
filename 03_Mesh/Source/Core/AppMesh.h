#pragma once
#include "Core/Application.h"
#include "Renderer/Renderer.h"

class AppMesh : public BaseApplication
{
public:
    AppMesh();

protected:
    void Init() override;
    void Cleanup() override;
    void Update() override;
    void Render() override;
};
