#include "Pipeline.h"

#include "Renderer.h"

#include "Technique.h"

#include <chrono>

namespace lava
{
  void Pipeline::Create()
  {
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSet();
  }

  void Pipeline::Destroy()
  {
  }

  void Pipeline::Bind(VkCommandBuffer _commandBuffer)
  {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

    PerObject ubo = {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    //ubo.model *= glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
    ubo.view = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 800 / (float)600, 0.1f, 200.0f);
    ubo.proj[1][1] *= -1;

    uniformBuffer.update(&ubo, sizeof(ubo));
    
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
  }
}