#include "render/Device.h"
#include "graphics/Camera.h"
#include "imgui/Imgui.h"
#include "render/UniformBuffers.h"

#include <Logger.hpp>
#include <EnumStringConversor.hpp>

namespace
{
  const std::vector<const char*> sValidationLayers = { "VK_LAYER_LUNARG_standard_validation" };
  const std::vector<const char*> sDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  bool ChecksValidationLayersupport()
  {
    uint32_t lLayerCount;
    vkEnumerateInstanceLayerProperties(&lLayerCount, nullptr);

    std::vector<VkLayerProperties> lAvailableLayers(lLayerCount);
    vkEnumerateInstanceLayerProperties(&lLayerCount, lAvailableLayers.data());

    for (const char* layerName : sValidationLayers)
    {
      bool lLayerFound = false;
      for (const auto& layerProperties : lAvailableLayers)
      {
        if (strcmp(layerName, layerProperties.layerName) == 0)
        {
          lLayerFound = true;
          break;
        }
      }

      if (!lLayerFound)
        return false;
    }

    return true;
  }

  VkResult CreateDebugReportCallbackEXT
  (
    VkInstance instance, 
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugReportCallbackEXT* pCallback
  )
  {
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    return (func != nullptr) ? func(instance, pCreateInfo, pAllocator, pCallback) : VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
  {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
      func(instance, callback, pAllocator);
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
  (
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void*
  )
  {
    std::string lMsg("[Vulkan]");
    lMsg += " layerPrefix: ";
    lMsg += layerPrefix;
    lMsg += msg;
    lMsg += " obj: ";
    lMsg += std::to_string(obj);
    lMsg += " location: ";
    lMsg += std::to_string(location);
    lMsg += " code: ";
    lMsg += std::to_string(code);

#define CASE_DEBUG_REPOR(flag) case flag: lMsg += #flag; break;

    switch (objType)
    {
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT);
      CASE_DEBUG_REPOR(VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT);
    }

    switch (flags)
    {
    case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
    case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
      Log_Info(lMsg);
      break;
    case VK_DEBUG_REPORT_WARNING_BIT_EXT:
    case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
      Log_Warning(lMsg);
      break;
    case VK_DEBUG_REPORT_ERROR_BIT_EXT:
      Log_Error(lMsg);
      break;
    default:
      lavaAssert(false, "Invalid debug report flags");
    }
    return VK_FALSE;
  }

  bool CheckDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice)
  {
    uint32_t lExtensionsCount;
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr, &lExtensionsCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(lExtensionsCount);
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr, &lExtensionsCount, availableExtensions.data());

    std::set<std::string> lRequiredExtensions(sDeviceExtensions.begin(), sDeviceExtensions.end());
    for (const auto& extension : availableExtensions)
      lRequiredExtensions.erase(extension.extensionName);
    return lRequiredExtensions.empty();
  }
}

namespace lava
{
  void CDevice::Create(void* hwnd)
  {
    Log_Info
    ( 
      "Initializing LAVA Engine\n"
      "github:https://github.com/Vazquinhos\n"
      "email:vazquinhos@gmail.com\n"
    );

    CreateInstance();
    SetupDebugCallback();
    CreateSurface(hwnd);
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommandPool();
    mSwapChain.Create();
    CreateFences();
    CreateCommandBuffers();
    CreateRenderSemaphore();
    CreateDescriptorPool();
    CreateUniformBuffers();
  }

  void CDevice::Destroy()
  {
    vkDeviceWaitIdle(mDevice);

    vkWaitForFences(mDevice, static_cast<uint32_t>(mFences.size()), mFences.data(), VK_TRUE, UINT64_MAX);
    for (size_t i = 0; i < mFences.size(); ++i)
      vkDestroyFence(mDevice, mFences[i], nullptr);

    vkDestroySemaphore(mDevice, mRenderFinishedSemaphore, nullptr);

    mSwapChain.Destroy();
    
    vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

    DestroyUniformBuffers();

    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);

    vkDestroyDevice(mDevice, nullptr);
    DestroyDebugReportCallbackEXT(mInstance, mCallback, nullptr);
    vkDestroySurfaceKHR(mInstance, mSwapChain.mSurface, nullptr);
    vkDestroyInstance(mInstance, nullptr);
  }

  void CDevice::CreateInstance()
  {
    vkNew(VkApplicationInfo, VK_STRUCTURE_TYPE_APPLICATION_INFO, lAppInfo);

    lAppInfo.pApplicationName = "LavaEngine";
    lAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    lAppInfo.pEngineName = "LavaEngine";
    lAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    lAppInfo.apiVersion = VK_API_VERSION_1_0;

    vkNew(VkInstanceCreateInfo, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, lCreateInfo);
    lCreateInfo.pApplicationInfo = &lAppInfo;

    std::vector<const char*> lExtensions =
    {
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
      "VK_EXT_debug_report"
    };

    lCreateInfo.enabledExtensionCount = static_cast<uint32_t>(lExtensions.size());
    lCreateInfo.ppEnabledExtensionNames = lExtensions.data();
    lCreateInfo.enabledLayerCount = 0;

    if (ChecksValidationLayersupport())
    {
      lCreateInfo.enabledLayerCount = static_cast<uint32_t>(sValidationLayers.size());
      lCreateInfo.ppEnabledLayerNames = sValidationLayers.data();
    }
    else
    {
      Log_Warning("validation layers requested, but not available!");
    }

    vkCall(vkCreateInstance(&lCreateInfo, nullptr, &mInstance));
  }

  void CDevice::CreateSurface(void* hwnd)
  {
    vkNew(VkWin32SurfaceCreateInfoKHR, VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, lCreateInfo);
    lCreateInfo.hwnd = (HWND)hwnd;
    lCreateInfo.hinstance = ::GetModuleHandle(nullptr);
    vkCall(vkCreateWin32SurfaceKHR(mInstance, &lCreateInfo, nullptr, &mSwapChain.mSurface));
  }

  void CDevice::SetupDebugCallback()
  {
    vkNew(VkDebugReportCallbackCreateInfoEXT, VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT, lCreateInfo);
    lCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    lCreateInfo.pfnCallback = DebugCallback;
    vkCall(CreateDebugReportCallbackEXT(mInstance, &lCreateInfo, nullptr, &mCallback));
  }

  void CDevice::PickPhysicalDevice()
  {
    uint32_t lDeviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &lDeviceCount, nullptr);

    lavaAssert(lDeviceCount, "failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> lDevices(lDeviceCount);
    vkEnumeratePhysicalDevices(mInstance, &lDeviceCount, lDevices.data());

    for (size_t d = 0, lCountDevices = lDevices.size(); d < lCountDevices && mPhysicalDevice == nullptr; ++d)
    {
      const VkPhysicalDevice& lCurrentDevice = lDevices[d];
      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(lCurrentDevice, &queueFamilyCount, nullptr);

      std::vector<VkQueueFamilyProperties> lQueueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(lCurrentDevice, &queueFamilyCount, lQueueFamilies.data());

      for (size_t i = 0, lCountQueues = lQueueFamilies.size(); i < lCountQueues && mQueues.IsComplete() == false; ++i)
      {
        const VkQueueFamilyProperties& lCurrentQueueFamily = lQueueFamilies[i];
        if (lCurrentQueueFamily.queueCount > 0 && lCurrentQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
          mQueues.GraphicsFamily = static_cast<int>(i);

        VkBool32 lIsPresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(lCurrentDevice, static_cast<uint32_t>(i), mSwapChain.mSurface, &lIsPresentSupport);

        if (lCurrentQueueFamily.queueCount > 0 && lIsPresentSupport)
          mQueues.PresentFamily = static_cast<int>(i);
      }

      mSwapChain.SetPhysicalDevice(lCurrentDevice);

      if (CheckDeviceExtensionSupport(lCurrentDevice) && mSwapChain.IsComplete() )
      {
        VkPhysicalDeviceFeatures lSupportedFeatures;
        vkGetPhysicalDeviceFeatures(lCurrentDevice, &lSupportedFeatures);

        if (mQueues.IsComplete() && lSupportedFeatures.samplerAnisotropy )
          mPhysicalDevice = lCurrentDevice;
      }
    }

    lavaAssert(mPhysicalDevice, "failed to find a suitable GPU!");
  }

  void CDevice::CreateLogicalDevice()
  {
      std::vector<VkDeviceQueueCreateInfo> lQueueCreateInfos;
      std::set<int> lUniqueQueueFamilies = { mQueues.GraphicsFamily, mQueues.PresentFamily };

      float queuePriority = 1.0f;
      for (int queueFamily : lUniqueQueueFamilies)
      {
        vkNew(VkDeviceQueueCreateInfo, VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, lQueueCreateInfo);
        lQueueCreateInfo.queueFamilyIndex = queueFamily;
        lQueueCreateInfo.queueCount = 1;
        lQueueCreateInfo.pQueuePriorities = &queuePriority;
        lQueueCreateInfos.push_back(lQueueCreateInfo);
      }

      vkNew(VkDeviceCreateInfo, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, lCreateInfo);
      lCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(lQueueCreateInfos.size());
      lCreateInfo.pQueueCreateInfos = lQueueCreateInfos.data();

      VkPhysicalDeviceFeatures lDeviceFeatures = {};
      lDeviceFeatures.samplerAnisotropy = VK_TRUE;
      lCreateInfo.pEnabledFeatures = &lDeviceFeatures;

      lCreateInfo.enabledExtensionCount = static_cast<uint32_t>(sDeviceExtensions.size());
      lCreateInfo.ppEnabledExtensionNames = sDeviceExtensions.data();

      lCreateInfo.enabledLayerCount = 0;
      if (ChecksValidationLayersupport())
      {
        lCreateInfo.enabledLayerCount = static_cast<uint32_t>(sValidationLayers.size());
        lCreateInfo.ppEnabledLayerNames = sValidationLayers.data();
      }
      else
      {
        Log_Warning("validation layers requested, but not available!");
      }

      vkCall(vkCreateDevice(mPhysicalDevice, &lCreateInfo, nullptr, &mDevice));

      vkGetDeviceQueue(mDevice, mQueues.GraphicsFamily, 0, &mQueues.Graphics);
      vkGetDeviceQueue(mDevice, mQueues.PresentFamily, 0, &mQueues.Present);
  }

  void CDevice::CreateCommandPool()
  {
    vkNew(VkCommandPoolCreateInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, lPoolInfo)

    lPoolInfo.queueFamilyIndex = mQueues.GraphicsFamily;
    lPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCall(vkCreateCommandPool(mDevice, &lPoolInfo, nullptr, &mCommandPool));
  }

  VkFormat CDevice::FindSupportedFormat(const std::vector<VkFormat>& aCandidates, VkImageTiling aTiling, VkFormatFeatureFlags aFeatures)
  {
    for (VkFormat lCurrentFormat : aCandidates)
    {
      VkFormatProperties lProps;
      vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, lCurrentFormat, &lProps);

      if (aTiling == VK_IMAGE_TILING_LINEAR && (lProps.linearTilingFeatures & aFeatures) == aFeatures)
        return lCurrentFormat;

      if (aTiling == VK_IMAGE_TILING_OPTIMAL && (lProps.optimalTilingFeatures & aFeatures) == aFeatures)
        return lCurrentFormat;
    }

    return VK_FORMAT_UNDEFINED;
  }

  uint32_t CDevice::FindMemoryType(uint32_t aTypeFilter, VkMemoryPropertyFlags aProperties)
  {
    VkPhysicalDeviceMemoryProperties lMemProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &lMemProperties);

    for (uint32_t i = 0; i < lMemProperties.memoryTypeCount; ++i)
    {
      if ((aTypeFilter & (1 << i)) && (lMemProperties.memoryTypes[i].propertyFlags & aProperties) == aProperties)
        return i;
    }

    lavaAssert(false, "unable to find a correct memory type");
    return 0;
  }

  VkCommandBuffer CDevice::BeginSingleExecutionCommand()
  {
    VkCommandBuffer lCommandBuffer = nullptr;

    vkNew( VkCommandBufferAllocateInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, lAllocInfo)
    lAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    lAllocInfo.commandPool = mCommandPool;
    lAllocInfo.commandBufferCount = 1;
    vkCall(vkAllocateCommandBuffers(mDevice, &lAllocInfo, &lCommandBuffer));

    vkNew(VkCommandBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, lBeginInfo)
    lBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCall(vkBeginCommandBuffer(lCommandBuffer, &lBeginInfo));

    return lCommandBuffer;
  }

  void CDevice::EndSingleExecutionCommand(VkCommandBuffer aCommandBuffer)
  {
    vkEndCommandBuffer(aCommandBuffer);

    vkNew(VkSubmitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO, lSubmitInfo);
    lSubmitInfo.commandBufferCount = 1;
    lSubmitInfo.pCommandBuffers = &aCommandBuffer;

    vkQueueSubmit(mQueues.Graphics, 1, &lSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(mQueues.Graphics);

    vkFreeCommandBuffers(mDevice, mCommandPool, 1, &aCommandBuffer);
  }

  void CDevice::CreateCommandBuffers()
  {
    const size_t lNumberOfFrameBuffers = mSwapChain.GetNumberOfFramebuffers();
    mCommandBuffers.resize(lNumberOfFrameBuffers);

    vkNew(VkCommandBufferAllocateInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, lAllocInfo);
    lAllocInfo.commandPool = mCommandPool;
    lAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    lAllocInfo.commandBufferCount = static_cast<uint32_t>(lNumberOfFrameBuffers);

    vkCall(vkAllocateCommandBuffers(mDevice, &lAllocInfo, mCommandBuffers.data()));

    for (size_t i = 0; i < mCommandBuffers.size(); ++i)
    {
      vkNew(VkCommandBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, lBeginInfo);

      lBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

      vkBeginCommandBuffer(mCommandBuffers[i], &lBeginInfo);

      vkNew(VkRenderPassBeginInfo, VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, lRenderPassInfo);

      lRenderPassInfo.renderPass = mSwapChain.GetRenderPass();
      lRenderPassInfo.framebuffer = mSwapChain.GetFramebufferByIdx(i);
      lRenderPassInfo.renderArea.offset = { 0, 0 };
      lRenderPassInfo.renderArea.extent = mSwapChain.GetExtent2D();

      std::array<VkClearValue, 2> clearValues = {};
      clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.1f };
      clearValues[1].depthStencil = { 1.0f, 0 };

      lRenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      lRenderPassInfo.pClearValues = clearValues.data();

      vkCmdBeginRenderPass(mCommandBuffers[i], &lRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

      vkCmdEndRenderPass(mCommandBuffers[i]);

      vkCall(vkEndCommandBuffer(mCommandBuffers[i]));

      vkNew(VkSubmitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO, lSubmitInfo);

      lSubmitInfo.commandBufferCount = 1;
      lSubmitInfo.pCommandBuffers = &mCommandBuffers[0];

      vkCall(vkQueueSubmit(mQueues.Graphics, 1, &lSubmitInfo, mFences[i]));
    }
  }

  void CDevice::CreateFences()
  {
    vkNew(VkFenceCreateInfo, VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, lCreateInfo);

    // We need this so we can wait for them on the first try
    lCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    const size_t lNumberOfFrameBuffers = mSwapChain.GetNumberOfFramebuffers();
    mFences.resize(lNumberOfFrameBuffers);

    for (size_t i = 0; i < lNumberOfFrameBuffers; ++i)
      vkCall(vkCreateFence(mDevice, &lCreateInfo, nullptr, &mFences[i]));
    
    vkResetFences(mDevice, static_cast<uint32_t>(mFences.size()), mFences.data());
  }

  void CDevice::CreateRenderSemaphore()
  {
    vkNew(VkSemaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, lCreateInfo);
    vkCall(vkCreateSemaphore(mDevice, &lCreateInfo, nullptr, &mRenderFinishedSemaphore));
  }

  void CDevice::BeginFrame(const CCamera& aRenderingCamera)
  {
    mCurrentImageIndex = mSwapChain.GetNextImage();
    
    vkWaitForFences(mDevice, 1, &mFences[mCurrentImageIndex], VK_TRUE, UINT64_MAX);
    vkResetFences(mDevice, 1, &mFences[mCurrentImageIndex]);

    vkNew(VkCommandBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, lBeginInfo);
    lBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(mCommandBuffers[mCurrentImageIndex], &lBeginInfo);

    vkNew(VkRenderPassBeginInfo, VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, lRenderpassInfo);

    lRenderpassInfo.renderPass = mSwapChain.GetRenderPass();
    lRenderpassInfo.framebuffer = mSwapChain.GetFramebufferByIdx(mCurrentImageIndex);
    lRenderpassInfo.renderArea.offset = { 0, 0 };
    lRenderpassInfo.renderArea.extent = mSwapChain.GetExtent2D();

    std::array<VkClearValue, 2> clearValues = {};
    if (aRenderingCamera.GetClearMode() == CCamera::ClearMode::eSolidColor)
    {
      const float3& lClearColor = aRenderingCamera.GetClearColor();
      std::memcpy(&clearValues[0].color, &lClearColor.x, sizeof(float) * 4);
    }
    else
      clearValues[0].color = { 0.25f, 0.25f, 0.25f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    lRenderpassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    lRenderpassInfo.pClearValues = clearValues.data();

    VkViewport viewport = {};
    const float4& lCameraViewport = aRenderingCamera.GetViewport();
    viewport.x = lCameraViewport.x;
    viewport.y = lCameraViewport.y;
    viewport.width = lCameraViewport.z;
    viewport.height = lCameraViewport.w;
    vkCmdSetViewport(mCommandBuffers[mCurrentImageIndex], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent = VkExtent2D{ (uint32_t)viewport.width, (uint32_t)viewport.height };
    vkCmdSetScissor(mCommandBuffers[mCurrentImageIndex], 0, 1, &scissor);

    vkCmdBeginRenderPass(mCommandBuffers[mCurrentImageIndex], &lRenderpassInfo, VK_SUBPASS_CONTENTS_INLINE);
  }

  void CDevice::EndFrame()
  {
    vkCmdEndRenderPass(mCommandBuffers[mCurrentImageIndex]);

    vkCall(vkEndCommandBuffer(mCommandBuffers[mCurrentImageIndex]));

    vkNew(VkSubmitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO, lSubmitInfo);

    VkSemaphore waitSemaphores[] = { mSwapChain.GetImageAvailableSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    lSubmitInfo.waitSemaphoreCount = 1;
    lSubmitInfo.pWaitSemaphores = waitSemaphores;
    lSubmitInfo.pWaitDstStageMask = waitStages;

    lSubmitInfo.commandBufferCount = 1;
    lSubmitInfo.pCommandBuffers = &mCommandBuffers[mCurrentImageIndex];

    VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphore };
    lSubmitInfo.signalSemaphoreCount = 1;
    lSubmitInfo.pSignalSemaphores = signalSemaphores;

    vkCall(vkQueueSubmit(mQueues.Graphics, 1, &lSubmitInfo, mFences[mCurrentImageIndex]));

    vkNew(VkPresentInfoKHR, VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, lPresentInfo);

    lPresentInfo.waitSemaphoreCount = 1;
    lPresentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR lSwapChains[] = { mSwapChain.mSwapChain };
    lPresentInfo.swapchainCount = 1;
    lPresentInfo.pSwapchains = lSwapChains;

    lPresentInfo.pImageIndices = &mCurrentImageIndex;

    vkCall( vkQueuePresentKHR(mQueues.Present, &lPresentInfo) );

    /*
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }
    */

    vkQueueWaitIdle(mQueues.Present);
  }

  void CDevice::CreateDescriptorPool()
  {
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

    vkCall(vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool));
  }

  void CDevice::CreateUniformBuffers()
  {
    mPerFrameUniformBuffer.create
    (
      mDevice,
      mPhysicalDevice,
      sizeof(TransformUBO),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    mLightsUniformBuffer.create
    (
      mDevice,
      mPhysicalDevice,
      sizeof(LightUBO),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
  }

  void CDevice::DestroyUniformBuffers()
  {
    mPerFrameUniformBuffer.destroy(mDevice);
    mLightsUniformBuffer.destroy(mDevice);
  }
}
