#include "render/Device.h"

#include "Logger.hpp"

namespace lava
{
  namespace
  {
    const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    bool checkValidationLayerSupport()
    {
      uint32_t layerCount;
      vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

      std::vector<VkLayerProperties> availableLayers(layerCount);
      vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

      for (const char* layerName : validationLayers)
      {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers)
        {
          if (strcmp(layerName, layerProperties.layerName) == 0)
            layerFound = true;
            break;
        }

        if (!layerFound)
          return false;
      }

      return true;
    }
  }

  void Device::create()
  {
    createInstance();
  }

  void Device::destroy()
  {
    vkDestroyInstance(mInstance, nullptr);
  }

  void Device::createInstance()
  {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "LavaEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "LavaEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions =
    {
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
      "VK_EXT_debug_report"
    };
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;

    if (checkValidationLayerSupport())
    {
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
      Log_Warning("validation layers requested, but not available!");
    }

    vkCall(vkCreateInstance(&createInfo, nullptr, &mInstance));
  }
}
