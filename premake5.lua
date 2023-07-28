local baseproject_name = "DX11Sandbox"
build_dir = "%{wks.location}/build/%{prj.name}_$(Configuration)_$(Platform)"
intermediate_dir = "%{wks.location}/build/intermediate/%{prj.name}_$(Configuration)_$(Platform)"
project_dir = "%{wks.location}/%{prj.name}"

function AddSourceFiles(directory_path)
    files
    {
        ("./" .. directory_path .. "/**.h"),
        ("./" .. directory_path .. "/**.hpp"),
        ("./" .. directory_path .. "/**.cpp"),
        ("./" .. directory_path .. "/**.inl"),
        ("./" .. directory_path .. "/**.c"),
        ("./" .. directory_path .. "/**.cs")
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

print("Generating Project: " .. baseproject_name)
project (baseproject_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "StaticLib"

    AddSourceFiles(baseproject_name)
    includedirs { ("$(SolutionDir)/" .. baseproject_name .. "/Source/") }

    pchheader ("Core.h")
    pchsource ("./" .. baseproject_name .. "/Source/Core/Core.cpp")
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

local project_name = "01_HelloTriangle"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
        "4189"  -- local variable initalized but not referenced
    }

    AddSTB()
    AddSpdlog()
    AddSDL2()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

    filter {}

project_name = "02_TexturedCube"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
        "4189"  -- local variable initalized but not referenced
    }

    AddSTB()
    AddSpdlog()
    AddSDL2()

    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }

    filter {}

project_name = "03_Mesh"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
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

project_name = "04_ShaderReflection"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
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

project_name = "05_Models"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
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

project_name = "06_Lighting"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
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

project_name = "07_NormalMapping"
print("Generating Project: " .. project_name)
project (project_name)
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)
    kind "ConsoleApp"

    links { (baseproject_name) }
    includedirs { ("./" .. baseproject_name .. "/Source/") }

    AddSourceFiles(project_name)
    includedirs { "$(ProjectDir)" }
    includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }

    pchheader ("AppCore.h")
    pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
    forceincludes  { "AppCore.h" }

    disablewarnings
    {
        "4100", -- unreferenced formal parameter
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

project_name = "08_ShadowMapping"
    print("Generating Project: " .. project_name)
    project (project_name)
        location (project_dir)
        targetdir (build_dir)
        objdir (intermediate_dir)
        kind "ConsoleApp"
    
        links { (baseproject_name) }
        includedirs { ("./" .. baseproject_name .. "/Source/") }
    
        AddSourceFiles(project_name)
        includedirs { "$(ProjectDir)" }
        includedirs { ("$(SolutionDir)/" .. project_name .. "/Source/") }
    
        pchheader ("AppCore.h")
        pchsource ("./" .. project_name .. "/Source/Core/AppCore.cpp")
        forceincludes  { "AppCore.h" }
    
        disablewarnings
        {
            "4100", -- unreferenced formal parameter
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

project_name = "Shaders"
print("Generating Project: " .. project_name)
project (project_name)
    kind "Utility"
    location (project_dir)
    targetdir (build_dir)
    objdir (intermediate_dir)

    files
    {
        "%{wks.location}/**.hlsl*"
    }

project_name = "RegenerateProjectFiles"
print("Generating Project: " .. project_name)
project (project_name)
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
