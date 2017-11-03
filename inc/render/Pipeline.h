#pragma  once

#include  "lava.h"

namespace  lava
{
  class Pipeline
  {
  public:
    Pipeline() = default;
    virtual ~Pipeline() = default;
    void bind(VkCommandBuffer commandBuffer, const VkDescriptorSet* pDescriptorSets)
    {
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, pDescriptorSets, 0, nullptr);
    }

    virtual void destroy(VkDevice _device)
    {
      vkDestroyDescriptorSetLayout(_device, descriptorSetLayout, nullptr);
      vkDestroyPipeline(_device, pipeline, nullptr);
      vkDestroyPipelineLayout(_device, layout, nullptr);
    }

    VkDescriptorSetLayout descriptorLayout() const { return descriptorSetLayout;  }

  protected:
    VkDescriptorSetLayout descriptorSetLayout; 
    VkPipelineLayout layout;
    VkPipeline pipeline;
  };

  class SceneObjectPipeline : public Pipeline
  {
  public:
    SceneObjectPipeline() = default;
    virtual ~SceneObjectPipeline() = default;

    virtual void create(VkDevice _device, VkRenderPass _renderPass, VkExtent2D _swapChainExtent );
  };
}