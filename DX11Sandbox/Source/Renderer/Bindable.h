#pragma once

class IBindable
{
public:
    virtual void Bind() = 0;
    virtual ~IBindable() = default;
};