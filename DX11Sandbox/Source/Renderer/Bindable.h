#pragma once
#include "GraphicsContext.h"

class IBindable
{
public:
    virtual void Bind() = 0;
    virtual ~IBindable() = default;
};