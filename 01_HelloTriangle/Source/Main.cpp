#include "Core/AppHelloTriangle.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppHelloTriangle();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
