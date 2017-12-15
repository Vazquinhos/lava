#if 1
#include "lava.h"
#include "render/Window.h"

#include "ecs/World.h"
#include "ecs/Component.h"
#include "ecs/Entity.h"

#include "render/Device.h"
#include "render/Buffer.h"
#include "render/Geometry.h"
#include "textures/Samplers.h"
#include "textures/Texture.h"
#include "imgui/imgui_impl.h"
#include "imgui/imgui_bindings.h"

#include "render/Shader.h"
#include "render/Pipeline.h"
#include "graphics/visuals/VisualGizmo.h"

#include "graphics/Transform.h"

#include "render/Vertex.h"
#include "graphics/visuals/VisualMesh.h"

#include "graphics/AABB.h"
#include "graphics/Light.h"
#include "graphics/Camera.h"
#include "graphics/CameraController.h"

#include "ImGuizmo.h"


static int WIDTH = 1280;
static int HEIGHT = 720;

lava::VisualMesh mesh;


struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

UniformBufferObject ubo = {};
bool initedMatrix = false;

std::vector<lava::DebugVertex> debugVertices;

std::vector<uint32_t> debugIndices;

lava::AABB meshAABB;

lava::CCamera* camera = nullptr;
lava::CCameraController cameraController;

class HelloTriangleApplication {
public:
  void run()
  {
    initWindow();
    initVulkan();
    
    lava::Entity* entity = lava::World::getInstance().getNewEntity("cube");
    entity->addComponent<lava::Transform>();
    lava::VisualMesh* visualMesh = entity->addComponent<lava::VisualMesh>();
    visualMesh->create(device, physicalDevice, commandPool, graphicsQueue, "meshes/cube.obj");

    entity = lava::World::getInstance().getNewEntity("mitsuba");
    entity->addComponent<lava::Transform>();
    visualMesh = entity->addComponent<lava::VisualMesh>();
    visualMesh->create(device, physicalDevice, commandPool, graphicsQueue, "meshes/mitsuba-sphere.obj");

    entity = lava::World::getInstance().getNewEntity("point light");
    lava::Transform * trsf = entity->addComponent<lava::Transform>();
    trsf->position() = glm::vec3(10);
    trsf->recompose();
    entity->addComponent<lava::Light>();

    entity = lava::World::getInstance().getNewEntity("main camera");
    entity->addComponent<lava::Transform>();
    camera = entity->addComponent<lava::CCamera>();
    camera->eye() = glm::vec3(2, 0, 2);
    camera->lookAt() = glm::vec3(0);
    camera->viewport() = glm::vec4(0.0f, 0.0f, WIDTH, HEIGHT);
    camera->fov() = 60.0f;

    cameraController.setControllCCamera(camera);

    mainLoop();
    cleanup();
  }

private:
  lava::Window* window;

  VkInstance instance;
  VkDebugReportCallbackEXT callback;
  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  struct
  {
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout layout;
    VkPipeline pipeline;
  }
  debug;

  lava::SceneObjectPipeline sceneObjectPipeline;
  std::vector<VkFence> fences;
  
  VkCommandPool commandPool;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  lava::CTexture texture;

  VkBuffer debugVertexBuffer;
  VkDeviceMemory debugVertexBufferMemory;
  VkBuffer debugIndexBuffer;
  VkDeviceMemory debugIndexBufferMemory;

  VkDescriptorPool descriptorPool;
  VkDescriptorSet descriptorSet;

  std::vector<VkCommandBuffer> commandBuffers;

  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;

  void initWindow()
  {
    window = new lava::Window("Vulkan", WIDTH, HEIGHT );
  }

  void initImGui()
  {
    // Setup ImGui binding
    lava::ImGuiInit_Data init_data = {};
    init_data.allocator = nullptr;
    lava::CDevice& lDevice = lava::CDevice::getInstance();
    init_data.gpu = lDevice.GetPhyiscalDevice();
    init_data.device = lDevice.GetLogicalDevice();;
    init_data.render_pass = renderPass;
    init_data.pipeline_cache = VK_NULL_HANDLE;
    //init_data.check_vk_result = check_vk_result;
    lava::ImGuiInit(window, &init_data);

    VkCommandBuffer commandBuffer = lava::CDevice::getInstance().BeginSingleExecutionCommand();
    lava::ImGuiCreateFontsTexture(commandBuffer);
    lava::CDevice::getInstance().EndSingleExecutionCommand(commandBuffer);
  }

  void initMesh()
  {
    std::string inputfile;
    int option = 3;
    float scale = 1.0f;
    switch (option)
    {
    case 1:
      inputfile = +"meshes/cube.obj";
      break;
    case 2:
      inputfile += "meshes/sponza.obj";
      scale = 0.05f;
      break;
    case 3:
      inputfile += "meshes/mitsuba-sphere.obj";
      scale = 1.0f;
      break;
    case 4:
      inputfile += "meshes/dragon.obj";
      scale = 2.0f;
      break;
    case 5:
      inputfile += "meshes/bunny.obj";
      scale = 1.0f;
      break;
    case 6:
      inputfile += "meshes/head.obj";
      scale = 2.0f;
      break;
    }

    mesh.create(device, physicalDevice, commandPool, graphicsQueue, inputfile );
  }

  void initVulkan()
  {
    lava::CDevice& lDevice = lava::CDevice::getInstance();
    lDevice.Create(window->GetHWND());

    device = lDevice.GetLogicalDevice();
    physicalDevice = lDevice.GetPhyiscalDevice();
    renderPass = lDevice.GetSwapChain().GetRenderPass();
    swapChainExtent = lDevice.GetSwapChain().GetExtent2D();
    commandPool = lDevice.GetCommandPool();

    sceneObjectPipeline.create(device, renderPass, swapChainExtent);

    initMesh();

    //initImGui();

    createTextureSampler();
    createTextureImage();
    createUniformBuffer();

    createDescriptorPool();
    createDescriptorSet();
  }

  void mainLoop()
  {
    while (!window->IsClosed()) {
      window->Update();
      
      cameraController.update(0.0016f);
      camera->updateMatrices();

      drawFrame();
    }

    vkDeviceWaitIdle(device);
  }

  void cleanup()
  {
    lava::ImGuiShutdown();
    lava::CDevice::getInstance().Destroy();

    lava::Samplers::destroy();
    texture.Destroy();
    
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    uniformBuffer.destroy(device);
    uniformBufferLight.destroy(device);
    
    mesh.destroy(device);

    delete window;
  }

  void createTextureImage()
  {
    texture.Create( "textures/uvchecker.png" );
    texture.SetSampler(lava::Samplers::sLinearSampler);
  }

  void createTextureSampler()
  {
    lava::Samplers::create();
  }

  struct LightUBO
  {
    glm::vec3 position;
    float beginRange;
    glm::vec3 direction;
    float endRange;
    glm::vec3 color;
    float intensity;
  };
  

  LightUBO uboLight;

  lava::Buffer uniformBuffer;
  lava::Buffer uniformBufferLight;
  void createUniformBuffer()
  {
    uniformBuffer.create
    (
      device,
      physicalDevice,
      sizeof(UniformBufferObject),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    uniformBufferLight.create
    (
      device,
      physicalDevice,
      sizeof(LightUBO),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
  }

  void createDebugDescriptorPool() {
    std::array<VkDescriptorPoolSize, 1> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &debug.descriptorPool) != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  void createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 3> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 1;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 1;
    

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  void createDescriptorSet() {
    VkDescriptorSetLayout layouts[] = { sceneObjectPipeline.descriptorLayout() };
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate descriptor set!");
    }

    VkDescriptorBufferInfo bufferInfo = uniformBuffer.descriptorInfo();
    VkDescriptorBufferInfo bufferInfoLight = uniformBufferLight.descriptorInfo();

    VkDescriptorImageInfo imageInfo = texture.GetDescriptor();
    
    std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &bufferInfoLight;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = descriptorSet;
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }

  void drawFrame()
  {
    lava::CDevice::getInstance().BeginFrame(*camera);
    
    lava::Transform* trsf = lava::World::getInstance().find("point light")->getComponent<lava::Transform>();

    lava::Light* light = lava::World::getInstance().find("point light")->getComponent<lava::Light>();

    uboLight.position = trsf->position();
    uboLight.direction = trsf->apply(glm::vec3(0, 1, 0));
    uboLight.beginRange = light->rangeAttenuation().x;
    uboLight.endRange = light->rangeAttenuation().y;
    uboLight.color = light->color();
    uboLight.intensity = light->intensity();
    
    uniformBufferLight.update(device, sizeof(LightUBO), &uboLight);

    ubo.view = camera->view();
    ubo.proj = camera->proj();

    lava::World& world = lava::World::getInstance();
    const size_t numEntities = world.length();
    for (size_t i = 0; i < numEntities; ++i)
    {
      lava::Entity* currentEntity = world.at(i);
      if (currentEntity->active())
      {
        lava::VisualMesh* visual = currentEntity->getComponent<lava::VisualMesh>();
        if (visual)
        {
          for (uint32_t j = 0, count = visual->geometryCount(); j < count; ++j)
          {
            const auto& geometry = visual->geometry(j);
            geometry->bind(lava::CDevice::getInstance().GetFrameCommandBuffer());

            ubo.model = currentEntity->getComponent<lava::Transform>()->transformation();
            
            sceneObjectPipeline.bind(lava::CDevice::getInstance().GetFrameCommandBuffer(), &descriptorSet);
            uniformBuffer.update(device, sizeof(UniformBufferObject), &ubo);
            geometry->render(lava::CDevice::getInstance().GetFrameCommandBuffer());
          }
        }
      }
    }
    lava::CDevice::getInstance().EndFrame();
  }
};

INT WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
  HelloTriangleApplication app;

  try {
    app.run();
  }
  catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

#endif