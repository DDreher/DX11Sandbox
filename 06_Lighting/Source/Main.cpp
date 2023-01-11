#include "Core/AppLighting.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppLighting();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
