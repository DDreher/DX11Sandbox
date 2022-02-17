#pragma once
#include "GraphicsContext.h"

class IBindable
{
public:
    virtual void Bind(GraphicsContext& context) = 0;
    virtual ~IBindable() = default;
};