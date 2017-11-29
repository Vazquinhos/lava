/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"
#include "render/Buffer.h"

namespace lava
{
  class Geometry
  {
  public:
    Geometry() = default;
    virtual ~Geometry() = default;

    virtual void render(VkCommandBuffer commandBuffer) = 0;
    virtual void bind(VkCommandBuffer commandBuffer) = 0;
    virtual void destroy(VkDevice _device) = 0;

    uint32_t& verticesCount() { return mNumVertices; }
    uint32_t& indicesCount() { return mNumIndices; }
  protected:
    Buffer mStagingBuffer;
    Buffer mVertexBuffer;
    Buffer mIndexBuffer;
    uint32_t mNumIndices;
    uint32_t mNumVertices;
  };

  template <typename VertexType, typename IndexType>
  class IndexedGeometry : public Geometry
  {
  public:
    IndexedGeometry() = default;
    virtual ~IndexedGeometry() = default;

    void create
    (
      VkDevice _device,
      VkPhysicalDevice _phyDevice,
      VkCommandPool _cmdPool, 
      VkQueue _queue,
      const std::vector< VertexType >& _vertices,
      const std::vector< IndexType >& _indices
    )
    {
      // Vertex Buffer
      VkDeviceSize bufferSize = sizeof(VertexType) * _vertices.size();
      mNumVertices = static_cast<uint32_t>(_vertices.size());
      createStagingBuffer(_device, _phyDevice, bufferSize, (void*)_vertices.data());
      mVertexBuffer.create(_device, _phyDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      copyBuffer(_device, _cmdPool, _queue, mStagingBuffer.buffer(), mVertexBuffer.buffer(), bufferSize);
      mStagingBuffer.destroy(_device);

      // Index Buffer
      mNumIndices = static_cast<uint32_t>(_indices.size());
      bufferSize = sizeof(IndexType) * _indices.size();
      createStagingBuffer(_device, _phyDevice, bufferSize, (void*)_indices.data());
      mIndexBuffer.create(_device, _phyDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      copyBuffer(_device, _cmdPool, _queue, mStagingBuffer.buffer(), mIndexBuffer.buffer(), bufferSize);
      mStagingBuffer.destroy(_device);
    }

    virtual void destroy(VkDevice _device)
    {
      mVertexBuffer.destroy(_device);
      mIndexBuffer.destroy(_device);
    }

    void bind(VkCommandBuffer commandBuffer)
    {
      VkBuffer vertexBuffers[] = { mVertexBuffer.buffer() };
      VkDeviceSize offsets[] = { 0 };
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

      vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    void render(VkCommandBuffer _commandBuffer)
    {
      vkCmdDrawIndexed(_commandBuffer, mNumIndices, 1, 0, 0, 0);
    }

  private:
    void createStagingBuffer
    (
      VkDevice _device,
      VkPhysicalDevice _phyDevice, 
      VkDeviceSize _bufferSize,
      void* _data
    )
    {
      mStagingBuffer.create
      (
        _device,
        _phyDevice,
        _bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
      );
      mStagingBuffer.update(_device, _bufferSize, _data);
    }

    void destroym_stagingBuffer(VkDevice _device)
    {
      mStagingBuffer.destroy(_device);
    }
  };

}