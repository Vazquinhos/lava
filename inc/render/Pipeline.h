#pragma  once

#include  "lava.h"

namespace  lava
{
  struct  Pipeline
  {
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout layout;
    VkPipeline pipeline;

    void create(VkDevice _device)
    {

    }

    void destroy(VkDevice _device)
    {

    }
  };
}