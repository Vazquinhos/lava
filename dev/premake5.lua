workspace "lava"
   language "C++"
   configurations { "Debug", "Release" }
   platforms { "x64" }
   location "vs"
   buildoptions '/std:c++latest'
   systemversion "10.0.15063.0"
   
   targetdir "$(SolutionDir)bin/%{cfg.buildcfg}"
   debugdir "../data"
   
   filter "configurations:Debug"
      defines { "DEBUG" }
	  rtti ("off")
	  characterset ("MBCS")
	  symbols "on"

   filter "configurations:Release"
      defines { "NO_DEBUG" }
	  rtti ("off")
      optimize "On"

group "tools"
project "meshc"
	kind "ConsoleApp"
	warnings "Extra" 
	files { "../tools/meshc/**.cpp", "../tools/meshc/**.h" }

	includedirs
	{
		"../tools/meshc/**.cpp",
		"../external/assimp/include/"
	}
	
	libdirs
	{
		"$(SolutionDir)lib/%{cfg.buildcfg}",
		"../external/assimp/lib64/"
	}
	
	links
	{
		"assimp",
	}

group "app"
project "lavaEngine"
	kind "WindowedApp"
	--kind "ConsoleApp"
	
	warnings "Extra" 
	
	files { "../src/**.cpp", "../inc/**.h", "../inc/**.hpp"}

	includedirs
	{
		"../external/vulkan/inc/",
		"../external/vulkanhpp/",
		"../external/glm/",
		"../external/tinyobj/",
		"../external/imgui/",
		"../external/assimp/include/",
		"../inc/",
	}
	
	libdirs
	{
		"$(SolutionDir)lib/%{cfg.buildcfg}",
		"../external/vulkan/bin/",
		"../external/assimp/lib64/"
	}
	
	links
	{
		"vulkan-1",
		"assimp",
		"imgui"
	}

group "external"
project "imgui"
	kind "StaticLib"
	files {"../external/imgui/**.h", "../external/imgui/**.cpp"}
	excludes {"../external/imgui/examples/**.**", "../external/imgui/extra_fonts/**.**"}

group "resources"
project "shaders"
	kind "StaticLib"
	files {"../data/**.frag", "../data/**.vert"}