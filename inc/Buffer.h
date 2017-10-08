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
		virtual ~Buffer()
		{
			vkDestroyBuffer(device(), mBuffer, nullptr);
			vkFreeMemory(device(), mMemory, nullptr);
		}

		Buffer& create()
		{
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = mSize;
			bufferCreateInfo.usage = mUsageFlags;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			vkCreateBuffer(device(), &bufferCreateInfo, nullptr, &mBuffer);
			assert(mBuffer != nullptr);

			VkMemoryRequirements	bufferMemoryRequirements;
			vkGetBufferMemoryRequirements(device(), mBuffer, &bufferMemoryRequirements);

			const std::vector<lava::MemoryTypeInfo>& heaps = mRenderer->GetHeaps();
			for (const auto& memoryInfo : heaps)
			{
				// The VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT communicates that the memory should be mapped so that the CPU(host) can access it.
				// The VK_MEMORY_PROPERTY_HOST_COHERENT_BIT requests that the writes to the memory by the host are visible to the device(and vice - versa)
				// without the need to flush memory caches.This just makes it a bit simpler to program, since it isn't necessary to call vkFlushMappedMemoryRanges
				// and vkInvalidateMappedMemoryRanges to make sure that the data is visible to the GPU.
				if (memoryInfo.hostVisible && memoryInfo.hostCoherent)
				{
					VkMemoryAllocateInfo memoryAllocateInfo = {};
					memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					memoryAllocateInfo.memoryTypeIndex = memoryInfo.index;
					memoryAllocateInfo.allocationSize = bufferMemoryRequirements.size;
					vkAllocateMemory(device(), &memoryAllocateInfo, nullptr, &mMemory);
					break;
				}
			}
			assert(mMemory != VK_NULL_HANDLE);

			vkBindBufferMemory(device(), mBuffer, mMemory, 0);
			return *this;
		}

		virtual void Bind(VkCommandBuffer) { assert(false); };
		Buffer& data(void * _data)
		{
			void* mapping = nullptr;
			vkMapMemory(device(), mMemory, 0, mSize, 0, &mapping);
			if (mapping)
			{
				std::memcpy(mapping, _data, mSize);
				vkUnmapMemory(device(), mMemory);
				mapping = nullptr;
			}
			return *this;
		}

		const VkBufferUsageFlagBits& usageFlags() const { return mUsageFlags; }
		Buffer& usageFlags(const VkBufferUsageFlagBits& _flags) { mUsageFlags = _flags; return *this; }
		
		const uint32_t& size() const { return mSize; }
		Buffer& size(const uint32_t& _size) { mSize = _size; return *this; }

		VkBuffer buffer() const { return mBuffer; }

		VkDevice device() const { return mRenderer->GetDevice(); }
		const Renderer& renderer() const { return *mRenderer; }
		Buffer&  renderer(Renderer& _renderer) { mRenderer = &_renderer; return *this; }

	protected:
		uint32_t	   mSize = 1;
		VkBuffer	   mBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mMemory = VK_NULL_HANDLE;
		VkBufferUsageFlagBits mUsageFlags;
		Renderer*	mRenderer = nullptr;
	};

	template< typename T >
	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer()
		{
			usageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		}
		virtual ~VertexBuffer() = default;
		VertexBuffer& count(const uint32_t& _count) { mCount = _count; size(_count * sizeof(T));  return *this; }
		virtual void Bind(VkCommandBuffer commandBuffer) { VkDeviceSize offsets[] = { 0 }; vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mBuffer, offsets); }
	private:
		uint32_t mCount;
	};

	class Index32Buffer : public Buffer
	{
	public:
		Index32Buffer()
		{
			usageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		}
		virtual ~Index32Buffer() = default;
		Index32Buffer& count(const uint32_t& _count) { mCount = _count; size(_count * sizeof(uint32_t)); return *this; }
		virtual void Bind(VkCommandBuffer commandBuffer) { vkCmdBindIndexBuffer(commandBuffer, mBuffer, 0, VK_INDEX_TYPE_UINT32); }
		void Draw(VkCommandBuffer commandBuffer) { vkCmdDrawIndexed(commandBuffer, mCount, 1, 0, 0, 0); }
	private:
		uint32_t mCount;
	};

	template< typename T >
	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer()
		{
			usageFlags(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			size(sizeof(T));
		}

		virtual ~UniformBuffer() = default;
	};
}