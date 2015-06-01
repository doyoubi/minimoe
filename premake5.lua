solution "minimoe"
    location "build"
    targetdir "bin"
    includedirs { "src" }
    configurations { "Debug", "Release" }

project "Compiler"
    location "build/Compiler"
    kind "ConsoleApp"
    language "C++"
    files { "src/**.h", "src/**.cpp" }

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        flags { "Symbols" }

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"
