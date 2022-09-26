#pragma once
#include "Core/Application.h"
#include "Renderer/Renderer.h"

class AppTexturedCube : public BaseApplication
{
public:
    AppTexturedCube();

protected:
    void Init() override;
    void Cleanup() override;
    void Update() override;
    void Render() override;
};
