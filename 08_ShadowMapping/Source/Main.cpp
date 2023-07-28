#include "Core/AppShadowMapping.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppShadowMapping();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
