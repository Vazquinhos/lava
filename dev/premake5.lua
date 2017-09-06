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
	
project "vkr"
	--kind "WindowedApp"
	kind "ConsoleApp"
	
	warnings "Extra" 
	
	files { "../src/**.cpp", "../src/**.h", "../src/**.hpp"}

	includedirs
	{
		"../external/vulkan/inc/",
	}
	
	libdirs
	{
		"$(SolutionDir)lib/%{cfg.buildcfg}",
		"../external/vulkan/bin/"
	}
	
	links
	{
		"vulkan-1",
	}