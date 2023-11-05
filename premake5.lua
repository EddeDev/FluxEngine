workspace "FluxEngine"
    configurations { "Debug", "Release", "Shipping" }
    architecture "x64"
    flags "MultiProcessorCompile"
    startproject "FluxEngine"

    targetdir ("Build/Binaries/%{cfg.buildcfg}/%{prj.name}")
    objdir ("Build/Intermediate/%{cfg.buildcfg}/%{prj.name}")

project "FluxEngine"
    language "C++"
    cppdialect "C++latest"
    location "Engine"
    staticruntime "Off"

    pchheader "FluxPCH.h"
    pchsource "Engine/Source/FluxPCH.cpp"

    files
    {
        "Engine/Source/**.cpp",
        "Engine/Source/**.h"
    }

    includedirs
    {
        "Engine/Source",
        "Engine/Libraries/spdlog/include"
    }

    filter "system:windows"
        systemversion "latest"
        defines "FLUX_PLATFORM_WINDOWS"

    filter "system:macosx"
        systemversion "latest"
        defines "FLUX_PLATFORM_MAC"

    filter "system:linux"
        systemversion "latest"
        defines "FLUX_PLATFORM_LINUX"

    filter "configurations:Debug"
        kind "ConsoleApp"
        defines "FLUX_BUILD_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        kind "ConsoleApp"
        defines { "FLUX_BUILD_RELEASE", "NDEBUG" }
        runtime "Release"
        optimize "On"

    filter "configurations:Shipping"
        kind "WindowedApp"
        defines { "FLUX_BUILD_SHIPPING", "NDEBUG" }
        runtime "Release"
        optimize "On"
        symbols "Off"