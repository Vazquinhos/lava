#pragma once

#include "lava.h"
#include "Debug.h"
#include "Renderer.h"

namespace lava
{
  class Buffer
  {
  public:
    Buffer() = default;
    virtual ~Buffer() = default;

    Buffer& create
    (
      VkDeviceSize _size,
      VkBufferUsageFlags _usage,
      VkMemoryPropertyFlags _properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    )
    {
      VkBufferCreateInfo  bufferInfo = {};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.size = _size;
      bufferInfo.usage = _usage;
      bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      vkCall(vkCreateBuffer(device(), &bufferInfo, nullptr, &vkBuffer));

      VkMemoryRequirements  memRequirements;
      vkGetBufferMemoryRequirements(device(), vkBuffer, &memRequirements);

      VkMemoryAllocateInfo  allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;
      allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, _properties);

      vkCall(vkAllocateMemory(device(), &allocInfo, nullptr, &bufferMemory));

      vkBindBufferMemory(device(), vkBuffer, bufferMemory, 0);
      return *this;
    }

    Buffer& destroy()
    {
      vkDestroyBuffer(device(), vkBuffer, nullptr);
      vkFreeMemory(device(), bufferMemory, nullptr);
      return *this;
    }

    Buffer& update(void* _data, VkDeviceSize _size)
    {
      void* mapping = nullptr;
      vkCall(vkMapMemory(device(), bufferMemory, 0, _size, 0, &mapping));
      memcpy(mapping, _data, _size);
      vkUnmapMemory(device(), bufferMemory);
      return *this;
    }

    VkBuffer buffer() const { return vkBuffer; }
    const VkBuffer* bufferPtr() const { return &vkBuffer; }

    VkDevice device() const { return mRenderer->GetDevice(); }
    const Renderer& renderer() const { return *mRenderer; }
    Buffer&  renderer(Renderer& _renderer) { mRenderer = &_renderer; return *this; }

  private:
    Renderer*       mRenderer = nullptr;
    VkBuffer        vkBuffer;
    VkDeviceMemory  bufferMemory;

    uint32_t  findMemoryType(uint32_t  typeFilter, VkMemoryPropertyFlags  properties)
    {
      VkPhysicalDeviceMemoryProperties  memProperties;
      vkGetPhysicalDeviceMemoryProperties(renderer().GetPhysicalDevice(), &memProperties);

      for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
      {
        if ((typeFilter  &  (1 << i)) && (memProperties.memoryTypes[i].propertyFlags  &  properties) == properties)
          return  i;
      }

      errorLog("failed  to  find  suitable  memory  type!");
      return 0;
    }
  };
}