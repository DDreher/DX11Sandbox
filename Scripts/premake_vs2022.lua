require "premake_modules"

local BaseProjectName = "DX11Sandbox"

function AddSourceFiles(DirectoryPath)
    files
    { 
        ("../" .. DirectoryPath .. "/**.h"),
		("../" .. DirectoryPath .. "/**.hpp"),
        ("../" .. DirectoryPath .. "/**.cpp"),
        ("../" .. DirectoryPath .. "/**.inl"),
		("../" .. DirectoryPath .. "/**.c"),
		("../" .. DirectoryPath .. "/**.cs")
    }
end

function IncludeModule(ProjectName, ModuleName)
	includedirs { ("../" .. ProjectName .. "/Source/" .. ModuleName) }
end

local WorkspaceName = "DX11Sandbox"
workspace (WorkspaceName)
	location "../"
	basedir "../"
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

project (BaseProjectName)
	location ("../" .. BaseProjectName)
	targetdir ("../Build/" .. BaseProjectName .. "$(Configuration)_$(Platform)")
	objdir "!../Build/Intermediate/$(ProjectName)_$(Configuration)_$(Platform)"
	kind "StaticLib"

	AddSourceFiles(BaseProjectName)
	includedirs { "$(ProjectDir)" }
	IncludeModule (BaseProjectName, "Core")
	IncludeModule (BaseProjectName, "Renderer")
	
	pchheader ("Core.h")
	pchsource ("../" .. BaseProjectName .. "/Source/Core/Core.cpp")
	forceincludes  { "Core.h" }

	disablewarnings 
	{
        "4100", -- unreferenced formal paramter
		"4189"  -- local variable initalized but not referenced
	}

	AddSTB()
	AddTinyObjLoader()
	AddSpdlog()
	AddSDL2()

	filter "files:**/ThirdParty/**.*"
		flags "NoPCH"
		disablewarnings { "4100" }

local ProjectName = "01_HelloTriangle"
project (ProjectName)
	location ("../" .. ProjectName )
	targetdir ("../Build/" .. ProjectName .. "$(Configuration)_$(Platform)")
	objdir "!../Build/Intermediate/$(ProjectName)_$(Configuration)_$(Platform)"
	kind "ConsoleApp"

	links { (BaseProjectName) }
	includedirs { ("../" .. BaseProjectName .. "/Source/") }

	AddSourceFiles(ProjectName)
	includedirs { "$(ProjectDir)" }
	IncludeModule (ProjectName, "Core")
	IncludeModule (ProjectName, "Renderer")
	
	pchheader ("AppCore.h")
	pchsource ("../" .. ProjectName .. "/Source/Core/AppCore.cpp")
	forceincludes  { "AppCore.h" }

	disablewarnings 
	{
		"4100", -- unreferenced formal paramter
		"4189"  -- local variable initalized but not referenced
	}

	AddSTB()
	AddTinyObjLoader()
	AddSpdlog()
	AddSDL2()

	filter "files:**/ThirdParty/**.*"
		flags "NoPCH"
		disablewarnings { "4100" }

local ProjectName = "02_TexturedCube"
project (ProjectName)
	location ("../" .. ProjectName )
	targetdir ("../Build/" .. ProjectName .. "$(Configuration)_$(Platform)")
	objdir "!../Build/Intermediate/$(ProjectName)_$(Configuration)_$(Platform)"
	kind "ConsoleApp"

	links { (BaseProjectName) }
	includedirs { ("../" .. BaseProjectName .. "/Source/") }

	AddSourceFiles(ProjectName)
	includedirs { "$(ProjectDir)" }
	IncludeModule (ProjectName, "Core")
	IncludeModule (ProjectName, "Renderer")
	
	pchheader ("AppCore.h")
	pchsource ("../" .. ProjectName .. "/Source/Core/AppCore.cpp")
	forceincludes  { "AppCore.h" }

	disablewarnings 
	{
        "4100", -- unreferenced formal paramter
		"4189"  -- local variable initalized but not referenced
	}

	AddSTB()
	AddTinyObjLoader()
	AddSpdlog()
	AddSDL2()

	filter "files:**/ThirdParty/**.*"
		flags "NoPCH"
		disablewarnings { "4100" }
