workspace "lava"
   language "C++"
   configurations { "Debug", "Release", "Final" }
   platforms { "x64" }
   location "vs"
   buildoptions '/std:c++latest'
   systemversion "10.0.16299.0"
   --systemversion "10.0.15063.0"
   
   targetdir "$(SolutionDir)bin/%{cfg.buildcfg}"
   debugdir "../data"
   
   filter "configurations:Debug"
      defines { "DEBUG", "ENABLE_LOGGING" }
	  --rtti ("off")
	  characterset ("MBCS")
	  symbols "on"

   filter "configurations:Release"
      defines { "NO_DEBUG", "ENABLE_LOGGING" }
	  --rtti ("off")
      optimize "On"

   filter "configurations:Final"
      defines { "NO_DEBUG" }
	  --rtti ("off")
      optimize "On"
	  
group "engine"
project "serialization"
	kind "StaticLib"
	files { "../src/serialization/**.cpp", "../inc/serialization/**.h", "../inc/serialization/**.hpp" }
	
project "engine"
	kind "StaticLib"
	files { "../src/lava.cpp", "../inc/lava**.h" }
	files { "../src/debug/**.cpp", "../inc/debug/**.h", "../inc/debug/**.hpp" }
	
	includedirs
	{
		"../external/vulkan/inc/",
		"../external/glm/",
		"../external/imgui/",
		"../external/imguizmo/",
		"../external/vazk/vazk",
		"../inc/",
	}
	
project "render"
	kind "StaticLib"
	files { "../src/render/**.cpp", "../inc/render/**.h", "../inc/render/**.hpp" }
	files { "../src/imgui/**.cpp", "../inc/imgui/**.h", "../inc/imgui/**.hpp" }
	includedirs
	{
		"../external/vulkan/inc/",
		"../external/glm/",
		"../external/imgui/",
		"../external/imguizmo/",
		"../external/vazk/vazk",
		"../inc/",
	}
	
project "graphics"
	kind "StaticLib"
	files { "../src/graphics/**.cpp", "../inc/graphics/**.h", "../inc/graphics/**.hpp" }
	includedirs
	{
		"../external/vulkan/inc/",
		"../external/glm/",
		"../external/vazk/vazk",
		"../inc/",
	}
	
project "resources"
	kind "StaticLib"
	files { "../src/resources/**.cpp", "../inc/resources/**.h", "../inc/resources/**.hpp" }
	includedirs
	{
		"../external/vulkan/inc/",
		"../external/glm/",
		"../external/vazk/vazk",
		"../inc/",
	}

project "ecs"
	kind "StaticLib"
	files { "../src/ecs/**.cpp", "../inc/ecs/**.h", "../inc/ecs/**.hpp" }
	includedirs
	{
		"../external/vulkan/inc/",
		"../external/glm/",
		"../external/vazk/vazk",
		"../inc/",
	}

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
	
	files { "../src/Main2.cpp" }

	includedirs
	{
		"../external/vulkan/inc/",
		"../external/vulkanhpp/",
		"../external/glm/",
		"../external/tinyobj/",
		"../external/imgui/",
		"../external/assimp/include/",
		"../external/imguizmo/",
		"../external/vazk/vazk",
		"../external/cereal/include",
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
		"imgui",
		"vazk",
		"render",
		"graphics",
		"engine",
		"ecs",
		"resources"
	}

group "external"
project "vazk"
	kind "StaticLib"
	files {"../external/vazk/vazk/**.hpp", "../external/vazk/vazk/**.cpp"}
	includedirs { "../external/vazk/vazk" }

project "imgui"
	kind "StaticLib"
	files {"../external/imgui/**.h", "../external/imgui/**.cpp", "../external/imguizmo/**.h", "../external/imguizmo/**.cpp"}
	excludes {"../external/imgui/examples/**.**", "../external/imgui/extra_fonts/**.**"}
	includedirs { "../external/imgui" }

group "resources"
project "shaders"
	kind "StaticLib"
	files {"../data/**.frag", "../data/**.vert"}