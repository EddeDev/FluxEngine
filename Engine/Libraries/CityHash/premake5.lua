project "CityHash"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "Off"

    files
    {
        "src/**.h",
        "src/**.cc"
    }

    filter "system:Windows"
        systemversion "latest"
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Shipping"
        runtime "Release"
        optimize "On"
        symbols "Off"