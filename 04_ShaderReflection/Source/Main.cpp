#include "Core/AppShaderReflection.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppShaderReflection();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
