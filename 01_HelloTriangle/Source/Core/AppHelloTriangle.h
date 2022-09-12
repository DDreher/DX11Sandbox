#pragma once
#include "Core/Application.h"
#include "Renderer/Renderer.h"

class AppHelloTriangle : public BaseApplication
{
public:
    AppHelloTriangle();

protected:
    void Init() override;
    void Cleanup() override;
    void Update(double dt) override;
    void Render() override;
};
