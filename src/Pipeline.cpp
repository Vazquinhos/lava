#include "Pipeline.h"

#include "Renderer.h"
#include "Technique.h"

namespace lava
{
  Pipeline::Pipeline(const Renderer& _renderer)
    : viewport(VkOffset2D({ 0,0 }), { _renderer.GetFrameBufferWidth(), _renderer.GetFrameBufferHeight() })
    , blending(false)
    , mDevice(_renderer.GetDevice())
  {
    mGraphicsPipelineCreateInfo = {};
    mGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    assert(mPipelineLayout != VK_NULL_HANDLE);

    mGraphicsPipelineCreateInfo.layout = mPipelineLayout;

    mGraphicsPipelineCreateInfo.renderPass = _renderer.GetRenderPass();
  }

  void Pipeline::Create()
  {
    // Set the input layout that will the shaders have
    VkVertexInputBindingDescription vertexInputBindingDescription = { 0, mOffset, VK_VERTEX_INPUT_RATE_VERTEX };

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mVertexInputAttributeDescription.size());
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = mVertexInputAttributeDescription.data();
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

    mGraphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;

    // Set the vertex topology
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.topology = mTopology;

    mGraphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;

    // Set blending, viewport, raster, depth and multisample configuration for this pipeline
    mGraphicsPipelineCreateInfo.pViewportState = viewport.GetVkPipelineViewportStateCreateInfo();
    mGraphicsPipelineCreateInfo.pColorBlendState = blending.GetVkPipelineColorBlendStateCreateInfo();
    mGraphicsPipelineCreateInfo.pRasterizationState = rasterizer.GetVkPipelineRasterizationStateCreateInfo();
    mGraphicsPipelineCreateInfo.pDepthStencilState = depthStencil.GetVkPipelineDepthStencilStateCreateInfo();
    mGraphicsPipelineCreateInfo.pMultisampleState = multisample.GetVkPipelineMultisampleStateCreateInfo();

    // Shaders
    mGraphicsPipelineCreateInfo.stageCount = mTechnique->GetNumStages();
    mGraphicsPipelineCreateInfo.pStages = mTechnique->GetVkPipelineShaderStageCreateInfo();

    //mGraphicsPipelineCreateInfo.setLayoutCount = 1;
    //mGraphicsPipelineCreateInfo.pSetLayouts = &(_uniformBuffer.GetDescriptorSetLayout());

    vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfo, nullptr, &mPipeline);
  }

  Pipeline::~Pipeline()
  {
    ShutDown();
  }

  template <typename ...VertexFlags>
  void SetInputVertexLayout(uint32_t _binding, VertexFlags&&... _flags)
  {
    static const uint32_t size = sizeof...(VertexFlags);
    mVertexInputAttributeDescription.resize(size);
    mOffset = 0;
    std::vector<VertexFlag> flags = { _flags... };
    for (uint32_t location = 0; location < size; ++location)
    {
      vVertexInputAttributeDescription[location].binding = _binding;
      vVertexInputAttributeDescription[location].location = location;
      vVertexInputAttributeDescription[location].offset = offset;

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
      vVertexInputAttributeDescription[location].format = format;
    }
  }

  void Pipeline::ShutDown()
  {
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
  }

  void Pipeline::Bind(VkCommandBuffer _commandBuffer)
  {
    vkCmdBindPipeline(_commandBuffer, mPipelineBindPoint, mPipeline);
  }
}