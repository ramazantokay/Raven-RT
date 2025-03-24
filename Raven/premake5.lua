project "Raven"
    kind "StaticLib"
    language "C++"
    -- targetdir "bin/%{cfg.buildcfg}"
    staticruntime "off"

    pchheader "ravenpch.h"
    pchsource "src/ravenpch.cpp"
    
    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.inl",
        "src/**.cpp",
        "src/**.c",
        "src/**.cc"
    }


    includedirs
    {
        "src",
        "src/Math"
    }
    
    --removefiles 
    --{
    --    "src/Math/Vector3.h" 
    --}

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"
        defines { "RAVEN_PLATFORM_WINDOWS" }

    filter "system:linux"
        cppdialect "gnu++2a"
        links { "pthread" } 
        systemversion "latest"
        defines { "RAVEN_PLATFORM_LINUX" }


    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"