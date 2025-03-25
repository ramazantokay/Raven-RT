project "Nest"
    kind "ConsoleApp"
    language "C++"
    targetname ("raytracer")
    -- targetdir "bin/%{cfg.buildcfg}"
    staticruntime "off"
    
    files
    {
      "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "src/**.c",
        "src/**.cc"
    }
    
    includedirs
    {
        "src",
        -- Include Raven
        "../Raven/src/Math",
        "../Raven/src"
    }
    
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    links
    {
        "Raven"
    }

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"
        defines { }

    filter "system:linux"
        cppdialect "gnu++2a"
        systemversion "latest"
        links { "pthread" } 
        defines { }

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