#pragma  once

#include  "lava.h"

#include  "Viewport.h"
#include  "Blending.h"
#include  "Rasterizer.h"
#include  "MultiSample.h"
#include  "DepthStencil.h"
#include  "Buffer.h"
#include  "Technique.h"

namespace  lava
{
  class  Technique;
  class  Renderer;

  class  Pipeline
  {
  public:
    std::vector<Buffer*>  mUniformBuffers;
    struct  PerObject {
      glm::mat4  model;
      glm::mat4  view;
      glm::mat4  proj;
    };

    PerObject  object;
  public:
    Pipeline() = default;
    virtual  ~Pipeline() = default;

    VkDevice  device()  const { return  mRenderer->GetDevice(); }
    const  Renderer&  renderer()  const { return  *mRenderer; }
    Pipeline&         renderer(Renderer&  _renderer) { mRenderer = &_renderer;  return  *this; }

    const  VertexFlags& flags()  const { return  mVertexFlags; }
    Pipeline&           flags(const  VertexFlags&  _flags) { mVertexFlags = _flags;  return  *this; }

    const  Technique&  technique()  const { return  *mTechnique; }
    Pipeline&          technique(Technique*  _technique) { mTechnique = _technique;  return  *this; }

    const  Blending&  blending()  const { return  mBlending; }
    Pipeline&         blending(const  Blending&  _blending) { mBlending = _blending;  return  *this; }

    const  DepthStencil&  depthStencil()  const { return  mDepthStencil; }
    Pipeline&             depthStencil(const  DepthStencil&  _ds) { mDepthStencil = _ds;  return  *this; }

    const  Rasterizer&  rasterizer()  const { return  mRasterizer; }
    Pipeline&           rasterizer(const  Rasterizer&  _raster) { mRasterizer = _raster;  return  *this; }

    const  MultiSample& multisample()  const { return  mMultisample; }
    Pipeline&           multisample(const  MultiSample&  _multisample) { mMultisample = _multisample;  return  *this; }

    const  Viewport& viewport()  const { return  mViewport; }
    Pipeline&        viewport(const  Viewport&  _viewport) { mViewport = _viewport;  return  *this; }

    const  VkPrimitiveTopology&   topology()  const { return  mTopology; }
    Pipeline&	                    topology(const  VkPrimitiveTopology&  _topology) { mTopology = _topology;  return  *this; }

    void  Create();
    void  Bind(VkCommandBuffer  _commandBuffer);
    void  Destroy();

  public:
    VkDescriptorPool  descriptorPool;
    VkDescriptorSet  descriptorSet;

    VkDescriptorSetLayout  descriptorSetLayout;
    VkPipelineLayout  pipelineLayout;
    VkPipeline  graphicsPipeline;

    void  createDescriptorSetLayout()
    {
      VkDescriptorSetLayoutBinding  uboLayoutBinding = {};
      uboLayoutBinding.binding = 0;
      uboLayoutBinding.descriptorCount = 1;
      uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      uboLayoutBinding.pImmutableSamplers = nullptr;
      uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

      VkDescriptorSetLayoutCreateInfo  layoutInfo = {};
      layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      layoutInfo.bindingCount = 1;
      layoutInfo.pBindings = &uboLayoutBinding;

      if (vkCreateDescriptorSetLayout(device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  create  descriptor  set  layout!");
      }
    }

    void  createGraphicsPipeline()
    {
      //
      //  Set  the  input  layout  of  the  vertices
      //
      std::vector<  VkVertexInputAttributeDescription  >  descriptorInputAttributeDescriptions;
      uint32_t  offset = 0;
      uint32_t  location = 0;
      uint32_t  binding = 0;
      if ((mVertexFlags  &  lava::VertexFlags::ePosition) == lava::VertexFlags::ePosition)
      {
        descriptorInputAttributeDescriptions.resize(location + 1);
        descriptorInputAttributeDescriptions[location].location = location;
        descriptorInputAttributeDescriptions[location].binding = binding;
        descriptorInputAttributeDescriptions[location].offset = offset;
        descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
        offset += sizeof(float) * 3;
        location++;
      }

      if ((mVertexFlags  &  lava::VertexFlags::eNormal) == lava::VertexFlags::eNormal)
      {
        descriptorInputAttributeDescriptions.resize(location + 1);
        descriptorInputAttributeDescriptions[location].location = location;
        descriptorInputAttributeDescriptions[location].binding = binding;
        descriptorInputAttributeDescriptions[location].offset = offset;
        descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
        offset += sizeof(float) * 3;
        location++;
      }

      if ((mVertexFlags  &  lava::VertexFlags::eTangent) == lava::VertexFlags::eTangent)
      {
        descriptorInputAttributeDescriptions.resize(location + 1);
        descriptorInputAttributeDescriptions[location].location = location;
        descriptorInputAttributeDescriptions[location].binding = binding;
        descriptorInputAttributeDescriptions[location].offset = offset;
        descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
        offset += sizeof(float) * 3;
        location++;
      }

      if ((mVertexFlags  &  lava::VertexFlags::eBinormal) == lava::VertexFlags::eBinormal)
      {
        descriptorInputAttributeDescriptions.resize(location + 1);
        descriptorInputAttributeDescriptions[location].location = location;
        descriptorInputAttributeDescriptions[location].binding = binding;
        descriptorInputAttributeDescriptions[location].offset = offset;
        descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32B32_SFLOAT;
        offset += sizeof(float) * 3;
        location++;
      }

      if ((mVertexFlags  &  lava::VertexFlags::eUv) == lava::VertexFlags::eUv)
      {
        descriptorInputAttributeDescriptions.resize(location + 1);
        descriptorInputAttributeDescriptions[location].location = location;
        descriptorInputAttributeDescriptions[location].binding = binding;
        descriptorInputAttributeDescriptions[location].offset = offset;
        descriptorInputAttributeDescriptions[location].format = VK_FORMAT_R32G32_SFLOAT;
        offset += sizeof(float) * 2;
        location++;
      }

      debugLog(std::string("Number  of  locations  for  the  pipeline  ") + std::to_string(location));

      VkVertexInputBindingDescription  vertexInputBindingDescription = { 0,  offset,  VK_VERTEX_INPUT_RATE_VERTEX };

      VkPipelineVertexInputStateCreateInfo  vertexInputInfo = {};
      vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(descriptorInputAttributeDescriptions.size());
      vertexInputInfo.pVertexAttributeDescriptions = descriptorInputAttributeDescriptions.data();
      vertexInputInfo.vertexBindingDescriptionCount = 1;
      vertexInputInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

      VkPipelineInputAssemblyStateCreateInfo  inputAssembly = {};
      inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly.topology = mTopology;
      inputAssembly.primitiveRestartEnable = VK_FALSE;

      VkViewport  viewport = {};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)mRenderer->GetFrameBufferWidth();
      viewport.height = (float)mRenderer->GetFrameBufferHeight();
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D  scissor = {};
      scissor.offset = { 0,  0 };
      scissor.extent = VkExtent2D({ mRenderer->GetFrameBufferWidth(), mRenderer->GetFrameBufferHeight() });

      VkPipelineViewportStateCreateInfo  viewportState = {};
      viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo  rasterizer = {};
      rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizer.depthClampEnable = VK_FALSE;
      rasterizer.rasterizerDiscardEnable = VK_FALSE;
      rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizer.lineWidth = 1.0f;
      rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
      rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      rasterizer.depthBiasEnable = VK_FALSE;

      VkPipelineMultisampleStateCreateInfo  multisampling = {};
      multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampling.sampleShadingEnable = VK_FALSE;
      multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

      VkPipelineColorBlendAttachmentState  colorBlendAttachment = {};
      colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachment.blendEnable = VK_FALSE;

      VkPipelineColorBlendStateCreateInfo  colorBlending = {};
      colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlending.logicOpEnable = VK_FALSE;
      colorBlending.logicOp = VK_LOGIC_OP_COPY;
      colorBlending.attachmentCount = 1;
      colorBlending.pAttachments = &colorBlendAttachment;
      colorBlending.blendConstants[0] = 0.0f;
      colorBlending.blendConstants[1] = 0.0f;
      colorBlending.blendConstants[2] = 0.0f;
      colorBlending.blendConstants[3] = 0.0f;

      VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
      pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
      pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
      pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
      pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
      pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
      pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
      pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
      pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
      pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;

      VkPipelineLayoutCreateInfo  pipelineLayoutInfo = {};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.setLayoutCount = 1;
      pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

      if (vkCreatePipelineLayout(device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  create  pipeline  layout!");
      }

      const  uint32_t  stageCount = mTechnique->shaderCount();
      std::vector<VkPipelineShaderStageCreateInfo>  shaderStages(stageCount);
      for (uint32_t iStage = 0; iStage < stageCount; ++iStage)
      {
        const  auto&  shader = mTechnique->shaderAt(iStage);
        shaderStages[iStage].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[iStage].module = shader.second->GetVkShaderModule();
        shaderStages[iStage].pName = "main";
        shaderStages[iStage].stage = shader.first;
      }

      VkGraphicsPipelineCreateInfo  pipelineInfo = {};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.stageCount = stageCount;
      pipelineInfo.pStages = shaderStages.data();
      pipelineInfo.pVertexInputState = &vertexInputInfo;
      pipelineInfo.pInputAssemblyState = &inputAssembly;
      pipelineInfo.pViewportState = &viewportState;
      pipelineInfo.pRasterizationState = &rasterizer;
      pipelineInfo.pMultisampleState = &multisampling;
      pipelineInfo.pColorBlendState = &colorBlending;
      pipelineInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
      pipelineInfo.layout = pipelineLayout;
      pipelineInfo.renderPass = mRenderer->GetRenderPass();
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

      if (vkCreateGraphicsPipelines(device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  create  graphics  pipeline!");
      }
    }

    Buffer uniformBuffer;
    void  createUniformBuffer()
    {
      uniformBuffer
        .renderer(*mRenderer)
        .create(sizeof(PerObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    void  createDescriptorPool() {
      VkDescriptorPoolSize  poolSize = {};
      poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSize.descriptorCount = 1;

      VkDescriptorPoolCreateInfo  poolInfo = {};
      poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      poolInfo.poolSizeCount = 1;
      poolInfo.pPoolSizes = &poolSize;
      poolInfo.maxSets = 1;

      if (vkCreateDescriptorPool(device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  create  descriptor  pool!");
      }
    }

    void  createDescriptorSet() {
      VkDescriptorSetLayout  layouts[] = { descriptorSetLayout };
      VkDescriptorSetAllocateInfo  allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool;
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = layouts;

      if (vkAllocateDescriptorSets(device(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  allocate  descriptor  set!");
      }

      VkDescriptorBufferInfo  bufferInfo = {};
      bufferInfo.buffer = uniformBuffer.buffer();
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(PerObject);

      VkWriteDescriptorSet  descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSet;
      descriptorWrite.dstBinding = 0;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(device(), 1, &descriptorWrite, 0, nullptr);
    }

    uint32_t  findMemoryType(uint32_t  typeFilter, VkMemoryPropertyFlags  properties) {
      VkPhysicalDeviceMemoryProperties  memProperties;
      vkGetPhysicalDeviceMemoryProperties(mRenderer->GetPhysicalDevice(), &memProperties);

      for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter  &  (1 << i)) && (memProperties.memoryTypes[i].propertyFlags  &  properties) == properties) {
          return  i;
        }
      }

      throw  std::runtime_error("failed  to  find  suitable  memory  type!");
    }

    void  createBuffer(VkDeviceSize  size, VkBufferUsageFlags  usage, VkMemoryPropertyFlags  properties, VkBuffer&  buffer, VkDeviceMemory&  bufferMemory) {
      VkBufferCreateInfo  bufferInfo = {};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.size = size;
      bufferInfo.usage = usage;
      bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      if (vkCreateBuffer(device(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  create  buffer!");
      }

      VkMemoryRequirements  memRequirements;
      vkGetBufferMemoryRequirements(device(), buffer, &memRequirements);

      VkMemoryAllocateInfo  allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memRequirements.size;
      allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

      if (vkAllocateMemory(device(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw  std::runtime_error("failed  to  allocate  buffer  memory!");
      }

      vkBindBufferMemory(device(), buffer, bufferMemory, 0);
    }

    VkPipeline                    mPipeline = VK_NULL_HANDLE;
    VkPipelineLayout	mPipelineLayout;
    VkDescriptorSet		mDescriptorSet;
    std::vector<  VkDescriptorSetLayout  >  mDescriptorSetLayouts;
    VkDescriptorPool	mDescriptorPool;

    Viewport                mViewport;
    DepthStencil	mDepthStencil;
    Rasterizer		mRasterizer;
    MultiSample		mMultisample;
    Blending		mBlending;

    VertexFlags  mVertexFlags;
    VkPrimitiveTopology  mTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    Technique*    mTechnique;
    Renderer*    mRenderer;
  };
}