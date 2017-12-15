#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>

#include <vector>
#include <set>
#include <map>
#include <any>
#include <memory>
#include <functional>
#include <string>
#include <ios>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <array>

#include <Windows.h>

#pragma warning( push )
#pragma warning( disable : 4201)
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

typedef glm::vec2 float2;
typedef glm::vec3 float3;
typedef glm::vec4 float4;
typedef glm::mat4 float4x4;

#pragma warning( pop ) 

#include "debug/Debug.h"

#include <EnumStringConversor.hpp>

#ifdef _DEBUG

#define  vkCall(x) { static bool ignoreNextErrors = false; if( !ignoreNextErrors ) { ignoreNextErrors = lava::checkError(x, #x); } }
#define  lavaAssert(x, msg) { static bool ignoreNextAsserts = false; if( !ignoreNextAsserts ) { ignoreNextAsserts = lava::checkAssert(x, msg); } }

#else

#define  vkCall(x) x;
#define  lavaAssert(x, msg) {}

#endif

#define vkNew(VkStructure, VKEnum, lVariable) VkStructure lVariable = {}; lVariable.sType = VKEnum;

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

  enum MaterialType
  {
    eStandard = 1,
    eUnlit,
    eImageEffect
  };

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
    return VertexFlags(static_cast<VertexFlags>(static_cast<uint32_t>(bit0) | static_cast<uint32_t>(bit1)));
  }

  class Device;
  Device& deviceInstance();
  void createDevice();
  void destroyDevice();

  VkCommandBuffer beginSingleTimeCommands(VkDevice _device, VkCommandPool _cmdPool);
  void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkDevice _device, VkCommandPool _cmdPool, VkQueue _queue);
  void copyBuffer(VkDevice _device, VkCommandPool _cmdPool, VkQueue _queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void copyBufferToImage(VkDevice _device, VkCommandPool _cmdPool, VkQueue _queue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
}
