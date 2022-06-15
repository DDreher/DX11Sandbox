project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"
    configurations {"Debug", "ReleaseWithDebugInfo", "Release"}

    targetdir (build_dir)
    objdir (intermediate_dir)

    files
    {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        "imgui_demo.cpp",
        "imgui_impl_win32.cpp",
        "imgui_impl_win32.h",
        "imgui_impl_sdl.cpp",
        "imgui_impl_sdl.h",
        "imgui_impl_dx11.cpp",
        "imgui_impl_dx11.h"
    }

    AddSDL2()

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:DebugRender"
        runtime "Debug"
        symbols "on"

    filter "configurations:ReleaseWithDebugInfo"
        runtime "Release"
        symbols "on"
        optimize "On"

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Full"

    filter {}
