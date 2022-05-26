function AddSTB()  
    defines { "MODULE_STB" }
    includedirs {
        "$(SolutionDir)/ThirdParty/stb/include/",
    } 

    filter {}
end

function AddSpdlog()  
    defines { "MODULE_SPDLOG" }
    includedirs {
        "$(SolutionDir)/ThirdParty/spdlog/include/",
    } 
        
    filter {}
end

function AddSDL2(isTarget)  
    defines { "MODULE_SDL2" }
	includedirs "$(SolutionDir)/ThirdParty/SDL2/include/"
    libdirs	"$(SolutionDir)/ThirdParty/SDL2/lib/x64"
    
    postbuildcommands
    { 
        "{COPY} \"$(SolutionDir)ThirdParty\\SDL2\\lib\\%{cfg.platform}\\SDL2.dll\" \"$(OutDir)\""
    }
    links
    {
        "SDL2.lib",
        "SDL2main.lib"
    }
end

function AddAssimp(isTarget)  
    defines { "MODULE_ASSIMP" }
	includedirs "$(SolutionDir)/ThirdParty/Assimp/include/"
    libdirs	"$(SolutionDir)/ThirdParty/Assimp/lib/x64"
    
    filter  "configurations:Debug"
        postbuildcommands
        { 
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.dll\" \"$(OutDir)\"",
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.pdb\" \"$(OutDir)\""
        }
        links
        {
            "assimp-vc143-mtd.lib"
        }

    filter  "configurations:DebugRender"
        postbuildcommands
        { 
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.dll\" \"$(OutDir)\"",
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mtd.pdb\" \"$(OutDir)\""
        }
        links
        {
            "assimp-vc143-mtd.lib"
        }

    filter  "configurations:Release"
        postbuildcommands
        { 
            "{COPY} \"$(SolutionDir)ThirdParty\\Assimp\\bin\\%{cfg.platform}\\assimp-vc143-mt.dll\" \"$(OutDir)\""
        }
        links
        {
            "assimp-vc143-mt.lib"
        }
end
