project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "Off"

    files
    {
        "include/glad/glad.h",
        "include/glad/glad_wgl.h",
        "include/KHR/khrplatform.h",
        "src/glad.c",
        "src/glad_wgl.c",
    }

    includedirs "include"

    filter "system:Windows"
        systemversion "latest"