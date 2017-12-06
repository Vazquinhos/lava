#include "render/Device.h"

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
    CreateInstance();
    SetupDebugCallback();
    CreateSurface(hwnd);
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommandPool();
    mSwapChain.Create();
  }

  void CDevice::Destroy()
  {
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
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
        vkGetPhysicalDeviceSurfaceSupportKHR(lCurrentDevice, i, mSwapChain.mSurface, &lIsPresentSupport);

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
    VkCommandBuffer lCommandBuffer;

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
}
