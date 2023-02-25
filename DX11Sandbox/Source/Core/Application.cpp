#include "Core/Application.h"

#include "SDL.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_sdl.h"

#include "Engine/Input.h"
#include "Renderer/IRenderer.h"
#include "Renderer/GraphicsContext.h"

void BaseApplication::Run()
{
    Init();
    MainLoop();
    Cleanup();
}

BaseApplication* BaseApplication::Get()
{
    return instance_;
}

float BaseApplication::GetTimestep()
{
    return (float)tick_timer_.elapsed_;
}

float BaseApplication::GetFPS()
{
    return 1.0f / GetTimestep();
}

void BaseApplication::Init()
{
    CHECK(instance_ == nullptr);
    LOG("Initializing application: {}", application_name_);
    instance_ = this;

    SDL_Init(SDL_INIT_VIDEO);
    InitWindow();

    gfx::Init(window_);
}

void BaseApplication::MainLoop()
{
    LOG("Entering Main Loop...");

    while (window_ != nullptr && window_->GetIsClosed() == false)
    {
        tick_timer_.Update();
        Update();
        Render();

        if (input::IsKeyDown(SDL_KeyCode::SDLK_ESCAPE))
        {
            DestroyWindow();
        }
    }
}

void BaseApplication::Cleanup()
{
    LOG("Tearing down application...");
    gfx::Shutdown();
    DestroyWindow();
    SDL_Quit();
}

void BaseApplication::InitWindow()
{
    window_ = new Window(application_name_, 1920, 1080);
    CHECK(window_ != nullptr);
}

void BaseApplication::DestroyWindow()
{
    if (window_ != nullptr)
    {
        delete window_;
        window_ = nullptr;
    }
}

void BaseApplication::Update()
{
    input::BeginNewFrame();
    input::ResetMousePosDelta();    // Have to manually reset, otherwise we only update on mouse moved event.

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        HandleSDLEvent(sdl_event);
    }

    world.Update();
}

void BaseApplication::HandleSDLEvent(const SDL_Event& sdl_event)
{
    input::HandleSDLEvent(sdl_event);

    if(window_ != nullptr)
    {
        window_->HandleSDLEvent(sdl_event);
    }

    if(input::IsKeyReleased(SDL_KeyCode::SDLK_i))
    {
        render_debug_ui_ = !render_debug_ui_;
    }
}

void BaseApplication::Render()
{
    gfx::renderer->Render();

    if(render_debug_ui_)
    {
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui::NewFrame();
        gfx::renderer->RenderUI();
        RenderUI();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    gfx::renderer->Present();
}

void BaseApplication::RenderUI()
{
    ImGui::Begin("Statistics");
    float ms_per_frame = GetTimestep();
    static float avg_ms_per_frame = 0.0f;
    avg_ms_per_frame = avg_ms_per_frame * 0.9f + ms_per_frame * 0.1f;
    static float min_ms_per_frame = ms_per_frame;
    min_ms_per_frame = std::min(min_ms_per_frame, ms_per_frame);
    static float max_ms_per_frame = ms_per_frame;
    max_ms_per_frame = std::max(max_ms_per_frame, ms_per_frame);
    static float min_fps = 144.0f;
    float fps = GetFPS();
    min_fps = std::min(min_fps, fps);

    ImGui::Text("Avg MS/Frame: %f", avg_ms_per_frame);
    ImGui::Text("Min MS/Frame: %f", min_ms_per_frame);
    ImGui::Text("Max MS/Frame: %f", max_ms_per_frame);
    ImGui::Text("FPS: %f", fps);
    ImGui::Text("Min FPS: %f", min_fps);
    ImGui::End();
}
