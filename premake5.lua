local BaseProjectName = "DX11Sandbox"
build_dir = "%{wks.location}/build/%{prj.name}_$(Configuration)_$(Platform)"
intermediate_dir = "%{wks.location}/build/intermediate/%{prj.name}_$(Configuration)_$(Platform)"
project_dir = "%{wks.location}/%{prj.name}"

function AddSourceFiles(DirectoryPath)
    files
    { 
        ("./" .. DirectoryPath .. "/**.h"),
        ("./" .. DirectoryPath .. "/**.hpp"),
        ("./" .. DirectoryPath .. "/**.cpp"),
        ("./" .. DirectoryPath .. "/**.inl"),
        ("./" .. DirectoryPath .. "/**.c"),
        ("./" .. DirectoryPath .. "/**.cs")
    }
end

include "ThirdParty"

workspace ("DX11Sandbox")
    location "./"
    basedir "./"
    language "C++"
    configurations {"Debug", "DebugRender", "ReleaseWithDebugInfo", "Release"}
    platforms {"x64"}
    warnings "default"
    characterset ("MBCS")
    rtti "Off"
    toolset "v143"
    cppdialect "C++latest"
    --flags {"FatalWarnings"}
    
    filter { "configurations:Debug" }
        runtime "Debug"
        defines { "_DEBUG" }
        symbols "On"
        optimize "Off"
        debugdir "$(SolutionDir)"

    filter { "configurations:DebugRender" }
        runtime "Debug"
        defines { "_DEBUG", "_RENDER_DEBUG" }
        symbols "On"
        optimize "Off"
        debugdir "$(SolutionDir)"

    filter { "configurations:ReleaseWithDebugInfo" }
        runtime "Release"
        defines { "_RELEASE", "NDEBUG" }
        symbols "On"
        optimize "Full"
        debugdir "$(SolutionDir)"

    filter { "configurations:Release" }
        runtime "Release"
        defines { "_RELEASE", "NDEBUG" }
        symbols "Off"
        optimize "Full"

group "Dependencies"
    include "ThirdParty/ImGui"
group ""

project (BaseProjectName)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "StaticLib"

    AddSourceFiles(BaseProjectName)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. BaseProjectName .. "/Source/") }
    
    pchheader ("Core.h")
    pchsource ("./" .. BaseProjectName .. "/Source/Core/Core.cpp")
    forceincludes  { "Core.h" }

    disablewarnings 
    {
        "4100", -- unreferenced formal parameter
        "4189"  -- local variable initalized but not referenced
    }

    AddSTB()
    AddSpdlog()
    AddSDL2()
    AddAssimp()
    AddImGui()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

local ProjectName = "01_HelloTriangle"
project (ProjectName)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (BaseProjectName) }
    includedirs { ("./" .. BaseProjectName .. "/Source/") }

    AddSourceFiles(ProjectName)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. ProjectName .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. ProjectName .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings 
    {
        "4100", -- unreferenced formal paramter
        "4189"  -- local variable initalized but not referenced
    }

    AddSTB()
    AddSpdlog()
    AddSDL2()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

ProjectName = "02_TexturedCube"
project (ProjectName)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (BaseProjectName) }
    includedirs { ("./" .. BaseProjectName .. "/Source/") }

    AddSourceFiles(ProjectName)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. ProjectName .. "/Source/") }
    
    pchheader ("AppCore.h")
    pchsource ("./" .. ProjectName .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings 
    {
        "4100", -- unreferenced formal paramter
        "4189"  -- local variable initalized but not referenced
    }

    AddSTB()
    AddSpdlog()
    AddSDL2()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

ProjectName = "03_Mesh"
    project (ProjectName)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (BaseProjectName) }
    includedirs { ("./" .. BaseProjectName .. "/Source/") }

    AddSourceFiles(ProjectName)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. ProjectName .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. ProjectName .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings 
    {
        "4100", -- unreferenced formal paramter
        "4189"  -- local variable initalized but not referenced
    }

    AddAssimp()
    AddSTB()
    AddSpdlog()
    AddSDL2()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

ProjectName = "04_ShaderReflection"
project (ProjectName)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (BaseProjectName) }
    includedirs { ("./" .. BaseProjectName .. "/Source/") }

    AddSourceFiles(ProjectName)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. ProjectName .. "/Source/") }
    
    pchheader ("AppCore.h")
    pchsource ("./" .. ProjectName .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings 
    {
        "4100", -- unreferenced formal paramter
        "4189"  -- local variable initalized but not referenced
    }

    AddAssimp()
    AddSTB()
    AddSpdlog()
    AddSDL2()
    AddImGui()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

project "RegenerateProjectFiles"
    kind "Utility"
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)

    files
    {
        "%{wks.location}/**premake5.lua"
    }

    postbuildmessage "Regenerating project files with Premake5..."
    postbuildcommands
    {
        "%{wks.location}/GenerateProjectFiles.bat"
    }
