#pragma once
#include "Core/Pool.h"
#include "Core/Handle.h"

class Texture;
struct GraphicsContext;

struct TestStruct
{
    TestStruct()
    {
        LOG("Creating TestStruct");
    };

    ~TestStruct()
    {
        LOG("Destroying TestStruct");
    }

    int val = 42;
};

class ResourceManager
{
public:
    Handle<Texture> CreateTexture(GraphicsContext* context, const char* path);

    Handle<TestStruct> CreateTestStruct();
    TestStruct* Get(Handle<TestStruct> handle);
    void Destroy(Handle<TestStruct> handle);

private:
    Pool<Texture, Texture> texture_pool_;
    Pool<TestStruct, TestStruct> test_pool_;
};
