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
	  
	  /*
	  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	  graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	  graphicsPipelineCreateInfo.renderPass = mRenderer->GetRenderPass();

	  //
	  // Set the input layout of the vertices
	  //
	  std::vector< VkVertexInputAttributeDescription > descriptorInputAttributeDescriptions;
	  uint32_t offset = 0;
	  uint32_t location = 0;
	  uint32_t binding = 0;
	  if ((mVertexFlags & lava::VertexFlags::ePosition) == lava::VertexFlags::ePosition)
	  {
		  descriptorInputAttributeDescriptions.resize(location + 1);
		  descriptorInputAttributeDescriptions[location].location = location;
		  descriptorInputAttributeDescriptions[location].binding = binding;
		  descriptorInputAttributeDescriptions[location].offset = offset;
		  descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
		  offset += sizeof(float) * 3;
		  location++;
	  }

	  if ((mVertexFlags & lava::VertexFlags::eNormal) == lava::VertexFlags::eNormal)
	  {
		  descriptorInputAttributeDescriptions.resize(location + 1);
		  descriptorInputAttributeDescriptions[location].location = location;
		  descriptorInputAttributeDescriptions[location].binding = binding;
		  descriptorInputAttributeDescriptions[location].offset = offset;
		  descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
		  offset += sizeof(float) * 3;
		  location++;
	  }

	  if ((mVertexFlags & lava::VertexFlags::eTangent) == lava::VertexFlags::eTangent)
	  {
		  descriptorInputAttributeDescriptions.resize(location + 1);
		  descriptorInputAttributeDescriptions[location].location = location;
		  descriptorInputAttributeDescriptions[location].binding = binding;
		  descriptorInputAttributeDescriptions[location].offset = offset;
		  descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
		  offset += sizeof(float) * 3;
		  location++;
	  }

	  if ((mVertexFlags & lava::VertexFlags::eBinormal) == lava::VertexFlags::eBinormal)
	  {
		  descriptorInputAttributeDescriptions.resize(location + 1);
		  descriptorInputAttributeDescriptions[location].location = location;
		  descriptorInputAttributeDescriptions[location].binding = binding;
		  descriptorInputAttributeDescriptions[location].offset = offset;
		  descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
		  offset += sizeof(float) * 3;
		  location++;
	  }

	  if ((mVertexFlags & lava::VertexFlags::eUv) == lava::VertexFlags::eUv)
	  {
		  descriptorInputAttributeDescriptions.resize(location + 1);
		  descriptorInputAttributeDescriptions[location].location = location;
		  descriptorInputAttributeDescriptions[location].binding = binding;
		  descriptorInputAttributeDescriptions[location].offset = offset;
		  descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32_SFLOAT;
		  offset += sizeof(float) * 2;
		  location++;
	  }

	  debugLog(std::string("Number of locations for the pipeline ") + std::to_string(location));

	  VkVertexInputBindingDescription vertexInputBindingDescription = { 0, offset, VK_VERTEX_INPUT_RATE_VERTEX };

	  VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
	  pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	  pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(descriptorInputAttributeDescriptions.size());
	  pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = descriptorInputAttributeDescriptions.data();
	  pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	  pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

	  graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;

	  //
	  // Set the vertex topology
	  //
	  VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
	  pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	  pipelineInputAssemblyStateCreateInfo.topology = mTopology;

	  graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;

	  //
	  // Set the shaders of the pipeline
	  //
	  graphicsPipelineCreateInfo.stageCount = mTechnique->GetNumStages();
	  graphicsPipelineCreateInfo.pStages = mTechnique->GetVkPipelineShaderStageCreateInfo();

	  //
	  // Set blending, viewport, raster, depth and multisample configuration for this pipeline
	  //
	  graphicsPipelineCreateInfo.pViewportState = mViewport.GetVkPipelineViewportStateCreateInfo();
	  graphicsPipelineCreateInfo.pColorBlendState = mBlending.GetVkPipelineColorBlendStateCreateInfo();
	  graphicsPipelineCreateInfo.pRasterizationState = mRasterizer.GetVkPipelineRasterizationStateCreateInfo();
	  graphicsPipelineCreateInfo.pDepthStencilState = mDepthStencil.GetVkPipelineDepthStencilStateCreateInfo();
	  graphicsPipelineCreateInfo.pMultisampleState = mMultisample.GetVkPipelineMultisampleStateCreateInfo();

	  //
	  // Create the pipeline layout
	  //
	  glm::vec3 colorRed(1, 0, 0);
	  lava::UniformBuffer<glm::vec3> colorBuffer;

	  colorBuffer
		  .renderer(*mRenderer)
		  .create()
		  .data(&colorRed);

	  const size_t idx = mDescriptorSetLayouts.size();
	  mDescriptorSetLayouts.resize(idx + 1);
	  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	  uboLayoutBinding.binding = 0;
	  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	  uboLayoutBinding.descriptorCount = 1;
	  uboLayoutBinding.stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	  uboLayoutBinding.pImmutableSamplers = nullptr;

	  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	  layoutInfo.bindingCount = 1;
	  layoutInfo.pBindings = &uboLayoutBinding;

	  if (vkCreateDescriptorSetLayout(renderer().GetDevice(), &layoutInfo, nullptr, &mDescriptorSetLayouts[idx]) != VK_SUCCESS)
		  errorLog("failed to create descriptor set layout!");

	  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size());
	  pipelineLayoutInfo.pSetLayouts = mDescriptorSetLayouts.data();
	  pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	  pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

	  if (vkCreatePipelineLayout(device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
		  errorLog("failed to create pipeline layout!");

	  graphicsPipelineCreateInfo.layout = mPipelineLayout;

	  //
	  // Create the descriptors pool
	  //
	  VkDescriptorPoolSize poolSize = {};
	  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	  poolSize.descriptorCount = 1;

	  VkDescriptorPoolCreateInfo poolInfo = {};
	  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	  poolInfo.poolSizeCount = 1;
	  poolInfo.pPoolSizes = &poolSize;
	  poolInfo.maxSets = 1;

	  if (vkCreateDescriptorPool(device(), &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
		  errorLog("failed to create descriptor pool!");
	  }

	  //
	  // Create the descriptors set
	  //
	  VkDescriptorSetAllocateInfo allocInfo = {};
	  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	  allocInfo.descriptorPool = mDescriptorPool;
	  allocInfo.descriptorSetCount = 1;
	  allocInfo.pSetLayouts = mDescriptorSetLayouts.data();

	  if (vkAllocateDescriptorSets(device(), &allocInfo, &mDescriptorSet) != VK_SUCCESS) {
		  throw std::runtime_error("failed to allocate descriptor set!");
	  }

	  VkDescriptorBufferInfo bufferInfo = {};
	  bufferInfo.buffer = colorBuffer.buffer();
	  bufferInfo.offset = 0;
	  bufferInfo.range = sizeof(glm::vec3);

	  VkWriteDescriptorSet descriptorWrite = {};
	  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	  descriptorWrite.dstSet = mDescriptorSet;
	  descriptorWrite.dstBinding = 0;
	  descriptorWrite.dstArrayElement = 0;
	  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	  descriptorWrite.descriptorCount = 1;
	  descriptorWrite.pBufferInfo = &bufferInfo;

	  vkUpdateDescriptorSets(device(), 1, &descriptorWrite, 0, nullptr);

	  //
	  // Finally create the pipeline
	  //
	  if (vkCreateGraphicsPipelines(device(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline) != VK_SUCCESS)
		  errorLog("failed to create the graphics pipeline");
		  */
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
	  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	  ubo.view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	  ubo.proj = glm::perspective(glm::radians(45.0f), 800 / (float)600, 0.1f, 10.0f);
	  ubo.proj[1][1] *= -1;

	  void* data;
	  vkMapMemory(device(), uniformBufferMemory, 0, sizeof(ubo), 0, &data);
	  memcpy(data, &ubo, sizeof(ubo));
	  vkUnmapMemory(device(), uniformBufferMemory);

	vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
  }
}