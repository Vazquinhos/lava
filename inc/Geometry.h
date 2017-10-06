#pragma once

#include "lava.h"
#include "Buffer.h"
#include "Renderer.h"

namespace lava
{
  template< class VertexType >
  class Geometry
  {
  public:
    Geometry(const Renderer& _renderer, VertexType* _vertexBuffer, const uint32_t _numVertices, uint32_t* _indexBuffer, const uint32_t _numIndices)
      : mVertexBuffer(_renderer, _numVertices)
      , mIndexBuffer(_renderer, _numIndices)
    {
      mVertexBuffer.Map();
      mVertexBuffer.Copy(_vertexBuffer);
      mVertexBuffer.Unmap();

      mIndexBuffer.Map();
      mIndexBuffer.Copy(_indexBuffer);
      mIndexBuffer.Unmap();
    }
    virtual ~Geometry() = default;

    void DrawIndexed(VkCommandBuffer commandBuffer)
    {
      mVertexBuffer.Bind(commandBuffer);
      mIndexBuffer.Bind(commandBuffer);
      mIndexBuffer.Draw(commandBuffer);
    }

  private:
    VertexBuffer<VertexType> mVertexBuffer;
    IndexBuffer mIndexBuffer;
  };

}