#include "Core/AppTexturedCube.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppTexturedCube();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
