#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>

#include <vector>
#include <set>
#include <memory>
#include <functional>
#include <string>
#include <fstream>      // std::ifstream
#include <ios>

#include <Windows.h>

#pragma warning( push )
#pragma warning( disable : 4201)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#pragma warning( pop ) 


#ifdef _DEBUG

#include "Debug.h"

#define  vkCall(x) { static bool ignoreNextErrors = false; if( !ignoreNextErrors ) { ignoreNextErrors = checkError(x, #x); } }

#else

#define  vkCall(x) x;

#endif

namespace lava
{
    static const int sQueueSlotCount = 3;
    struct MemoryTypeInfo
    {
      bool deviceLocal = false;
      bool hostVisible = false;
      bool hostCoherent = false;
      bool hostCached = false;
      bool lazilyAllocated = false;

      struct Heap
      {
        uint64_t size = 0;
        bool deviceLocal = false;
      };

      Heap heap;
      int index;
    };

    template <typename T>
    T RoundToNextMultiple(const T a, const T multiple)
    {
      return ((a + multiple - 1) / multiple) * multiple;
    }

	enum VertexFlags
	{
		ePosition = 0x00000001,
		eNormal = 0x00000002,
		eTangent = 0x00000004,
		eBinormal = 0x00000008,
		eUv = 0x00000010,
	};

	VULKAN_HPP_INLINE VertexFlags operator|(VertexFlags bit0, VertexFlags bit1)
	{
		return VertexFlags( static_cast<VertexFlags>( static_cast<uint32_t>(bit0) | static_cast<uint32_t>(bit1) ) );
	}
}
