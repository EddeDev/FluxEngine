workspace "FluxEngine"
    configurations { "Debug", "Release", "Shipping" }
    architecture "x64"
    flags "MultiProcessorCompile"
    startproject "FluxEngine"

    targetdir ("Build/Binaries/%{cfg.buildcfg}/%{prj.name}")
    objdir ("Build/Intermediate/%{cfg.buildcfg}/%{prj.name}")

VulkanSDK = os.getenv("VULKAN_SDK")

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
        "Engine/Libraries/spdlog/include",
        "Engine/Libraries/Glad/include",
        "Engine/Libraries/ImGui"
    }

    if VulkanSDK ~= nil then
        includedirs "%{VulkanSDK}/Include"
    end

    links
    {
        "Glad",
        "ImGui"
    }

    filter "system:windows"
        systemversion "latest"
        defines "FLUX_PLATFORM_WINDOWS"
        
        links
        {
            "opengl32.lib",
            "Gdi32.lib",
            "glu32.lib",

            "d3d11.lib",
            "d3d12.lib",
            "dxgi.lib",
            "dxguid.lib",
            "d3dcompiler.lib"
        }

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

        if VulkanSDK ~= nil then
            links
            {
                "%{VulkanSDK}/Lib/shaderc_sharedd.lib",

                "%{VulkanSDK}/Lib/glslangd.lib",

                "%{VulkanSDK}/Lib/spirv-cross-cored.lib",
                "%{VulkanSDK}/Lib/spirv-cross-hlsld.lib",
                "%{VulkanSDK}/Lib/spirv-cross-glsld.lib",
                "%{VulkanSDK}/Lib/SPIRV-Toolsd.lib"
            }
        end

    filter "configurations:Release"
        kind "ConsoleApp"
        defines { "FLUX_BUILD_RELEASE", "NDEBUG" }
        runtime "Release"
        optimize "On"

        if VulkanSDK ~= nil then
            links
            {
                "%{VulkanSDK}/Lib/shaderc_shared.lib",

                "%{VulkanSDK}/Lib/glslang.lib",

                "%{VulkanSDK}/Lib/spirv-cross-core.lib",
                "%{VulkanSDK}/Lib/spirv-cross-hlsl.lib",
                "%{VulkanSDK}/Lib/spirv-cross-glsl.lib"
            }
        end

    filter "configurations:Shipping"
        kind "WindowedApp"
        defines { "FLUX_BUILD_SHIPPING", "NDEBUG" }
        runtime "Release"
        optimize "On"
        symbols "Off"

        if VulkanSDK ~= nil then
            links
            {
                "%{VulkanSDK}/Lib/shaderc_shared.lib",

                "%{VulkanSDK}/Lib/glslang.lib",

                "%{VulkanSDK}/Lib/spirv-cross-core.lib",
                "%{VulkanSDK}/Lib/spirv-cross-hlsl.lib",
                "%{VulkanSDK}/Lib/spirv-cross-glsl.lib"
            }
        end

group "Libraries"
    include "Engine/Libraries/Glad"
    include "Engine/Libraries/ImGui"
group ""