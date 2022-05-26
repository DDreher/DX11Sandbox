#include "Core/AppMesh.h"

int main()
{
    Log::Init();
    BaseApplication* app = new AppMesh();
    app->Run();

    delete app;
    app = nullptr;

    return EXIT_SUCCESS;
}
