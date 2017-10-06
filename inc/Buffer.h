#pragma once

#include "lava.h"
#include "Renderer.h"

namespace lava
{
  template< typename T >
  class Buffer
  {
  public:
    Buffer(const Renderer& _renderer, uint32_t _count, const VkBufferUsageFlagBits _bits)
      : mDevice(_renderer.GetDevice())
      , mCount(_count)
      , mMapping(0)
    {
      VkBufferCreateInfo bufferCreateInfo = {};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = static_cast<uint32_t> (sizeof(T) * mCount);
      bufferCreateInfo.usage = _bits;

      vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mBuffer);
      assert(mBuffer != nullptr);

      vkGetBufferMemoryRequirements(mDevice, mBuffer, &mBufferMemoryRequirements);

      const std::vector<lava::MemoryTypeInfo>& heaps = _renderer.GetHeaps();
      for (const auto& memoryInfo : heaps)
      {
        if (memoryInfo.hostVisible && memoryInfo.hostCoherent)
        {
          VkMemoryAllocateInfo memoryAllocateInfo = {};
          memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
          memoryAllocateInfo.memoryTypeIndex = memoryInfo.index;
          memoryAllocateInfo.allocationSize = mBufferMemoryRequirements.size;
          vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mMemory);
          break;
        }
      }
      assert(mMemory != VK_NULL_HANDLE);

      vkBindBufferMemory(mDevice, mBuffer, mMemory, 0);
    }

    virtual ~Buffer()
    {
      vkDestroyBuffer(mDevice, mBuffer, nullptr);
      vkFreeMemory(mDevice, mMemory, nullptr);
    }

    virtual void Bind(VkCommandBuffer) {};

    void Map()
    {
      vkMapMemory(mDevice, mMemory, 0, sizeof(T) * mCount, 0, &mMapping);
    }

    void Copy(T* _data)
    {
      if (mMapping)
        std::memcpy(mMapping, _data, sizeof(T) * mCount);
    }

    void Unmap()
    {
      if (mMapping)
      {
        vkUnmapMemory(mDevice, mMemory);
        mMapping = nullptr;
      }
    }

  protected:
    VkDevice mDevice = VK_NULL_HANDLE;
    VkBuffer mBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mMemory = VK_NULL_HANDLE;
    void* mMapping = nullptr;
    VkMemoryRequirements mBufferMemoryRequirements;
    uint32_t mCount;
  };

  template< typename T >
  class VertexBuffer : public Buffer<T>
  {
  public:
    VertexBuffer(const Renderer& _renderer, uint32_t _count)
      : Buffer(_renderer, _count, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
    {
    }
    virtual ~VertexBuffer() = default;
    virtual void Bind(VkCommandBuffer commandBuffer)
    {
      VkDeviceSize offsets[] = { 0 };
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mBuffer, offsets);
    }
  private:
  };

  class IndexBuffer : public Buffer<uint32_t>
  {
  public:
    IndexBuffer(const Renderer& _renderer, uint32_t _count)
      : Buffer(_renderer, _count, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
    {
    }
    virtual ~IndexBuffer() = default;
    virtual void Bind(VkCommandBuffer commandBuffer)
    {
      vkCmdBindIndexBuffer(commandBuffer, mBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
    void Draw(VkCommandBuffer commandBuffer)
    {
      vkCmdDrawIndexed(commandBuffer, mCount, 1, 0, 0, 0);
    }
  private:
  };

  template< typename T >
  class UniformBuffer : public Buffer<T>
  {
  public:
    UniformBuffer(const Renderer& _renderer)
      : Buffer(_renderer, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {
      VkDescriptorSetLayoutBinding uboLayoutBinding = {};
      uboLayoutBinding.binding = 0;
      uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      uboLayoutBinding.descriptorCount = 1;
      uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
      uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

      VkDescriptorSetLayoutCreateInfo layoutInfo = {};
      layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      layoutInfo.bindingCount = 1;
      layoutInfo.pBindings = &uboLayoutBinding;

      vkCreateDescriptorSetLayout(_renderer.GetDevice(), &layoutInfo, nullptr, &mDescriptorSetLayout);
    }
    virtual ~UniformBuffer()
    {
      vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);
    }
    VkDescriptorSetLayout GetDescriptorSetLayout() const { return mDescriptorSetLayout; }
  private:
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
  };


}