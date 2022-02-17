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
    void Update(double dt) override;
    void Render() override;

private:
    UniquePtr<Renderer> renderer_;
};
