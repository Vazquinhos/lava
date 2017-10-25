#pragma once

#include "lava.h"

namespace lava
{
  struct Buffer
  {
    void create
    (
      VkDevice _device,
      VkPhysicalDevice _phyDevice,
      VkDeviceSize _size,
      VkBufferUsageFlags _usage,
      VkMemoryPropertyFlags _properties
    )
    {
      VkBufferCreateInfo bufferInfo = {};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.size = _size;
      bufferInfo.usage = _usage;
      bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      vkCall(vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer));

      VkMemoryRequirements memRequirements;
      vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

      VkMemoryAllocateInfo allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;

      VkPhysicalDeviceMemoryProperties memProperties;
      vkGetPhysicalDeviceMemoryProperties(_phyDevice, &memProperties);

      bool found = false;
      for (uint32_t i = 0; i < memProperties.memoryTypeCount && !found; ++i)
      {
        if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
        {
          allocInfo.memoryTypeIndex = i;
          found = true;
        }
      }
      lavaAssert(found, "Invalid memory type index");

      vkCall(vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory));

      vkBindBufferMemory(_device, buffer, bufferMemory, 0);
    }

    void destroy(VkDevice _device)
    {
      vkDestroyBuffer(_device, buffer, nullptr);
      vkFreeMemory(_device, bufferMemory, nullptr);
    }

    void copy(VkCommandBuffer _commandBuffer, VkDeviceSize _size, Buffer& other )
    {
      VkBufferCopy copyRegion = {};
      copyRegion.size = _size;
      vkCmdCopyBuffer(_commandBuffer, buffer, other.buffer, 1, &copyRegion);
    }

    void update(VkDevice _device, VkDeviceSize _size, void* _data )
    {
      void* mapping = nullptr;
      vkCall(vkMapMemory(_device, bufferMemory, 0, _size, 0, &mapping));
      memcpy(mapping, _data, _size);
      vkUnmapMemory(_device, bufferMemory);
    }

    VkBuffer        buffer;
    VkDeviceMemory  bufferMemory;
  };
}