function AddSTB()
    is_link_target = is_link_target or false
    filter {}
    defines { "MODULE_STB" }
    includedirs {
        "$(SolutionDir)/ThirdParty/stb/include/",
    }
end

function AddSpdlog()
    is_link_target = is_link_target or false
    filter {}
    defines { "MODULE_SPDLOG" }
    includedirs {
        "$(SolutionDir)/ThirdParty/spdlog/include/",
    }
end

function AddSDL2(is_link_target)
    is_link_target = is_link_target or false
    filter {}
    defines { "MODULE_SDL2" }
    includedirs "$(SolutionDir)/ThirdParty/SDL2/include/"

    if is_link_target then
        libdirs	"$(SolutionDir)/ThirdParty/SDL2/lib/x64"

        links
        {
            "SDL2.lib",
            "SDL2main.lib"
        }
    end

    postbuildcommands
    {
        "{COPY} \"$(SolutionDir)ThirdParty\\SDL2\\lib\\%{cfg.platform}\\SDL2.dll\" \"$(OutDir)\""
    }
end

function AddAssimp(is_link_target)
    is_link_target = is_link_target or false
    filter {}
    defines { "MODULE_ASSIMP" }
    includedirs "$(SolutionDir)/ThirdParty/Assimp/include/"

    if is_link_target then
        libdirs	"$(SolutionDir)/ThirdParty/Assimp/lib/x64"
    end

    filter  "configurations:Debug"
        postbuildcommands
        {
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.dll\" \"$(OutDir)\"",
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.pdb\" \"$(OutDir)\""
        }

        if is_link_target then
            links
            {
                "assimp-vc143-mtd.lib"
            }
        end

    filter  "configurations:DebugRender"
        postbuildcommands
        {
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.dll\" \"$(OutDir)\"",
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.pdb\" \"$(OutDir)\""
        }

        if is_link_target then
            links
            {
                "assimp-vc143-mtd.lib"
            }
        end

    filter  "configurations:ReleaseWithDebugInfo"
        postbuildcommands
        {
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mt.dll\" \"$(OutDir)\""
        }
        if is_link_target then
            links
            {
                "assimp-vc143-mt.lib"
            }
        end

    filter  "configurations:Release"
        postbuildcommands
        {
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mt.dll\" \"$(OutDir)\""
        }
        if is_link_target then
            links
            {
                "assimp-vc143-mt.lib"
            }
        end

    filter {}
end

function AddImGui(is_link_target)
    is_link_target = is_link_target or false
    filter {}
    print("-- Adding ImGui")
    defines { "MODULE_IMGUI" }
    includedirs "$(SolutionDir)/ThirdParty/ImGui/"

    if is_link_target then
        links
        {
            "ImGui"
        }
    end
end
