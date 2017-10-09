#pragma once

#include "lava.h"
#include "Buffer.h"
#include "Renderer.h"

namespace lava
{
  template< class VertexType >
  class IndexedGeometry
  {
  public:
    IndexedGeometry() = default;
    virtual ~IndexedGeometry() = default;

    VkDevice device() const { return mRenderer->GetDevice(); }
    const Renderer& renderer() const { return *mRenderer; }
    IndexedGeometry& renderer(Renderer& _renderer) { mRenderer = &_renderer; return *this; }

    IndexedGeometry& vertices(VertexType* _vertexBuffer, const uint32_t _numVertices)
    {
      numVertices = _numVertices;
      vertexBuffer
        .renderer(*mRenderer)
        .create(sizeof(VertexType) * _numVertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        .update(_vertexBuffer, sizeof(VertexType) * _numVertices);
      return *this;
    }

    IndexedGeometry& indices(uint32_t* _indexBuffer, const uint32_t _numIndices)
    {
      numIndices = _numIndices;
      indexBuffer
        .renderer(*mRenderer)
        .create(sizeof(uint32_t) * _numIndices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        .update(_indexBuffer, sizeof(uint32_t) * _numIndices);
      return *this;
    }

    IndexedGeometry& destroy()
    {
      indexBuffer.destroy();
      vertexBuffer.destroy();
      numIndices = 0u;
      numVertices = 0u;
      return *this;
    }

    IndexedGeometry& bind(VkCommandBuffer commandBuffer)
    {
      VkDeviceSize offsets[] = { 0 };
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer.bufferPtr(), offsets);
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
      return *this;
    }

    IndexedGeometry& drawIndexed(VkCommandBuffer commandBuffer)
    {
      vkCmdDrawIndexed(commandBuffer, numIndices, 1, 0, 0, 0);
      return *this;
    }

  public:
    Buffer vertexBuffer;
    Buffer indexBuffer;
    uint32_t numVertices;
    uint32_t numIndices;
    Renderer*  mRenderer;
  };

}