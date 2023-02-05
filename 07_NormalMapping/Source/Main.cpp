#include "Core/AppNormalMapping.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppNormalMapping();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
