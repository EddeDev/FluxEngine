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
        "Engine/Source/**.h",

        "Engine/Libraries/glm/glm/**.hpp",
        "Engine/Libraries/glm/glm/**.inl",

        "Engine/Libraries/stb_image/**.cpp",
        "Engine/Libraries/stb_image/**.h"
    }

    includedirs
    {
        "Engine/Source",
        "Engine/Libraries/spdlog/include",
        "Engine/Libraries/glm",
        "Engine/Libraries/stb_image"
    }
    
    defines
    {
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
    }

    if VulkanSDK ~= nil then
        includedirs "%{VulkanSDK}/Include"
        defines "FLUX_HAS_VULKAN_SDK"

        links
        {
            "%{VulkanSDK}/Lib/vulkan-1.lib",
            "%{VulkanSDK}/Lib/VkLayer_utils.lib"
        }
    else
        excludes
        {
            "Engine/Source/Flux/Runtime/Renderer/Vulkan/**.cpp",
            "Engine/Source/Flux/Runtime/Renderer/Vulkan/**.h"
        }
    end

    filter "system:windows"
        systemversion "latest"
        defines "FLUX_PLATFORM_WINDOWS"

        if VulkanSDK ~= nil then
            links "%{VulkanSDK}/Lib/dxcompiler.lib"
        end

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

                "%{VulkanSDK}/Lib/spirv-cross-core.lib",
                "%{VulkanSDK}/Lib/spirv-cross-hlsl.lib",
                "%{VulkanSDK}/Lib/spirv-cross-glsl.lib"
            }
        end