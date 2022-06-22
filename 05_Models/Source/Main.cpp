#include "Core/AppModels.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppModels();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
