#pragma once

#include "lava.h"

#include "Viewport.h"
#include "Blending.h"
#include "Rasterizer.h"
#include "MultiSample.h"
#include "DepthStencil.h"
#include "Buffer.h"

namespace lava
{
  class Technique;
  class Renderer;

  class Pipeline
  {
  public:
    Viewport viewport;
    Blending blending;
    Rasterizer rasterizer;
    DepthStencil depthStencil;
    MultiSample multisample;
  public:
    Pipeline(const Renderer& _renderer);
    virtual ~Pipeline();

    template <typename ...VertexFlags>
    void SetInputVertexLayout(uint32_t _binding, VertexFlags&&... _flags)
    {
      static const uint32_t size = sizeof...(VertexFlags);
      mVertexInputAttributeDescription.resize(size);
      mOffset = 0;
      std::vector<VertexFlag> flags = { _flags... };
      for (uint32_t location = 0; location < size; ++location)
      {
        mVertexInputAttributeDescription[location].binding = _binding;
        mVertexInputAttributeDescription[location].location = location;
        mVertexInputAttributeDescription[location].offset = mOffset;

        VkFormat format = VK_FORMAT_UNDEFINED;
        switch (flags[location])
        {
        case VertexFlag::Position:
        case VertexFlag::Normal:
        case VertexFlag::Tangent:
        case VertexFlag::Binormal:
          format = VK_FORMAT_R32G32B32_SFLOAT;
          mOffset += sizeof(float) * 3;
          break;
        case VertexFlag::Uv:
          format = VK_FORMAT_R32G32_SFLOAT;
          mOffset += sizeof(float) * 2;
          break;
        default:
          assert(false);
          break;
        }
        mVertexInputAttributeDescription[location].format = format;
      }
    }

    void SetTopology(const VkPrimitiveTopology _topology)
    {
      mTopology = _topology;
    }

    void SetTechnique(Technique* _technique)
    {
      mTechnique = _technique;
    }

    void Create();

    void ShutDown();
    
    void Bind(VkCommandBuffer _commandBuffer);

  private:
    VkDevice            mDevice = VK_NULL_HANDLE;
    VkPipeline          mPipeline = VK_NULL_HANDLE;
    VkPipelineLayout    mPipelineLayout = VK_NULL_HANDLE;
    std::vector< VkVertexInputAttributeDescription > mVertexInputAttributeDescription;
    uint32_t mOffset;
    VkGraphicsPipelineCreateInfo mGraphicsPipelineCreateInfo = {};
    VkPipelineBindPoint mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkPrimitiveTopology mTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    Technique* mTechnique;
    
  };
}