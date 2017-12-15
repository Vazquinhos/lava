#pragma once

#include "lava.h"

namespace lava
{
  class Buffer
  {
  public:
    Buffer() = default;
    virtual ~Buffer() = default;
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

      vkCall(vkCreateBuffer(_device, &bufferInfo, nullptr, &mBuffer));

      VkMemoryRequirements memRequirements;
      vkGetBufferMemoryRequirements(_device, mBuffer, &memRequirements);

      VkMemoryAllocateInfo allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;

      mSize = _size;

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

      vkCall(vkAllocateMemory(_device, &allocInfo, nullptr, &mMemory));

      vkBindBufferMemory(_device, mBuffer, mMemory, 0);
    }

    void destroy(VkDevice _device)
    {
      vkDestroyBuffer(_device, mBuffer, nullptr);
      vkFreeMemory(_device, mMemory, nullptr);
    }

    void copy(VkCommandBuffer _commandBuffer, VkDeviceSize _size, Buffer& other )
    {
      VkBufferCopy copyRegion = {};
      copyRegion.size = _size;
      vkCmdCopyBuffer(_commandBuffer, mBuffer, other.mBuffer, 1, &copyRegion);
    }

    void update(VkDevice _device, VkDeviceSize , void* _data )
    {
      void* mapping = nullptr;
      vkCall(vkMapMemory(_device, mMemory, 0, mSize, 0, &mapping));
      memcpy(mapping, _data, mSize);
      vkUnmapMemory(_device, mMemory);
    }

    VkDescriptorBufferInfo descriptorInfo() const
    {
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = mBuffer;
      bufferInfo.offset = 0;
      bufferInfo.range = mSize;
      return bufferInfo;
    }

    VkBuffer buffer() { return mBuffer; }

  private:
    VkDeviceSize    mSize;
    VkBuffer        mBuffer;
    VkDeviceMemory  mMemory;
  };
}