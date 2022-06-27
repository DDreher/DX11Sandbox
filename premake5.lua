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
        staticruntime "off"
        defines { "_DEBUG" }
        symbols "On"
        optimize "Off"
        debugdir "$(SolutionDir)"

    filter { "configurations:DebugRender" }
        runtime "Debug"
        staticruntime "off"
        defines { "_DEBUG", "_RENDER_DEBUG" }
        symbols "On"
        optimize "Off"
        debugdir "$(SolutionDir)"

    filter { "configurations:ReleaseWithDebugInfo" }
        runtime "Release"
        staticruntime "off"
        defines { "_RELEASE", "NDEBUG" }
        symbols "On"
        optimize "Full"
        debugdir "$(SolutionDir)"

    filter { "configurations:Release" }
        runtime "Release"
        staticruntime "off"
        defines { "_RELEASE", "NDEBUG" }
        symbols "Off"
        optimize "Full"

    filter {}

group "Dependencies"
    include "ThirdParty/ImGui"
group ""

print("Generating Project: " .. BaseProjectName)
project (BaseProjectName)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "StaticLib"

    AddSourceFiles(BaseProjectName)
    includedirs { ("$(SolutionDir)/" .. BaseProjectName .. "/Source/") }

    pchheader ("Core.h")
    pchsource ("./" .. BaseProjectName .. "/Source/Core/Core.cpp")
    forceincludes  { "Core.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
        "4189"  -- local variable initalized but not referenced
    }

    AddSTB(true)
    AddSpdlog(true)
    AddSDL2(true)
    AddAssimp(true)
    AddImGui(true)

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

    filter {}

local ProjectName = "01_HelloTriangle"
print("Generating Project: " .. ProjectName)
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

    filter {}

ProjectName = "02_TexturedCube"
print("Generating Project: " .. ProjectName)
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

    filter {}

ProjectName = "03_Mesh"
print("Generating Project: " .. ProjectName)
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

    filter {}

ProjectName = "04_ShaderReflection"
print("Generating Project: " .. ProjectName)
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

    filter {}

ProjectName = "05_Models"
print("Generating Project: " .. ProjectName)
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

    includedirs "$(SolutionDir)/ThirdParty/Assimp/include/"
    AddAssimp()
    AddSTB()
    AddSpdlog()
    AddSDL2()
    AddImGui()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

    filter {}

ProjectName = "RegenerateProjectFiles"
print("Generating Project: " .. ProjectName)
project (ProjectName)
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
