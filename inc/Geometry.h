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
		Geometry() = default;
		virtual ~Geometry() = default;

		VkDevice device() const { return mRenderer->GetDevice(); }
		const Renderer& renderer() const { return *mRenderer; }
		Geometry& renderer(Renderer& _renderer)
		{ 
			mRenderer = &_renderer; 
			mVertexBuffer.renderer(_renderer); 
			mIndexBuffer.renderer(_renderer); 
			return *this;
		}

		Geometry& vertices(VertexType* _vertexBuffer, const uint32_t _numVertices)
		{
			mVertexBuffer.count(_numVertices).create().data(_vertexBuffer);
			return *this;
		}

		Geometry& indices(uint32_t* _indexBuffer, const uint32_t _numIndices)
		{
			mIndexBuffer.count(_numIndices).create().data(_indexBuffer);
			return *this;
		}

		void DrawIndexed(VkCommandBuffer commandBuffer)
		{
			
			mIndexBuffer.Draw(commandBuffer);
		}

	public:
		Renderer*  mRenderer;
		VertexBuffer<VertexType> mVertexBuffer;
		Index32Buffer mIndexBuffer;
	};

}