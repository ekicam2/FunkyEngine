workspace "HelloWorld"
   configurations { "Debug", "Release" }
   platforms { "Win64" }

project "EngineProj"
    kind "ConsoleApp"
    language "C++"
    
    targetdir "build/%{cfg.architecture}/%{cfg.buildcfg}"
    flags { "NoPCH", "MultiProcessorCompile", "FatalCompileWarnings" }
    cppdialect "C++17"
    characterset "Unicode"
    
    files { "**.h", "**.c" , "**.hpp", "**.cpp" }


    currdir = io.popen"cd":read'*l'
    currdir = currdir..[[\Source]]
    includes = {}
    for dir in io.popen([[dir "]]..currdir..[[" /b /ad]]):lines() do  if dir ~= 'Shaders' then print('inserting: '..currdir..[[\]]..dir) table.insert(includes, currdir..[[\]]..dir) end end
    includedirs { includes }

    includedirs { "./Source/**" }

    libdirs { "Lib" }
    links { "SDL2", "SDL2Image" }

    filter "configurations:Debug"
      defines { "DEBUG" }
      optimize "Off"
      symbols "On"

   filter "configurations:Release"
      optimize "Full"