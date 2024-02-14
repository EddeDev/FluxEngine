project "stb_image"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"

    files
    {
        "include/stb_image.h",
        "include/stb_image_resize2.h",
        "include/stb_image_write.h",

        "src/stb_image.cpp"
    }

    includedirs "include"

    filter "system:Windows"
        systemversion "latest"