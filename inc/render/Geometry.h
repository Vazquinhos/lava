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

    virtual void bind(VkCommandBuffer commandBuffer) = 0;
    virtual void destroy(VkDevice _device) = 0;
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
      createStagingBuffer(_device, _phyDevice, bufferSize, (void*)_vertices.data());
      vertexBuffer.create(_device, _phyDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      copyBuffer(_device, _cmdPool, _queue, stagingBuffer.buffer, vertexBuffer.buffer, bufferSize);
      stagingBuffer.destroy(_device);

      // Index Buffer
      bufferSize = sizeof(IndexType) * _indices.size();
      createStagingBuffer(_device, _phyDevice, bufferSize, (void*)_indices.data());
      indexBuffer.create(_device, _phyDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      copyBuffer(_device, _cmdPool, _queue, stagingBuffer.buffer, indexBuffer.buffer, bufferSize);
      stagingBuffer.destroy(_device);
    }

    virtual void destroy(VkDevice _device)
    {
      vertexBuffer.destroy(_device);
      indexBuffer.destroy(_device);
    }

    void bind(VkCommandBuffer commandBuffer)
    {
      VkBuffer vertexBuffers[] = { vertexBuffer.buffer };
      VkDeviceSize offsets[] = { 0 };
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

      vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
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
      stagingBuffer.create
      (
        _device,
        _phyDevice,
        _bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
      );
      stagingBuffer.update(_device, _bufferSize, _data);
    }

    void destroyStagingBuffer(VkDevice _device)
    {
      stagingBuffer.destroy(_device);
    }

    Buffer stagingBuffer;
    Buffer vertexBuffer;
    Buffer indexBuffer;
  };

}