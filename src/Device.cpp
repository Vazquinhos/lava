#pragma once

#include "Device.h"

namespace lava
{
  namespace
  {
    const std::vector<const char*> validationLayers = {
      "VK_LAYER_LUNARG_standard_validation",
      "VK_LAYER_RENDERDOC_Capture",
    };

    const std::vector<const char*> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
  }

  Device& Device::create()
  {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    return *this;
  }

  Device& Device::destroy()
  {
    physicalDevice = VK_NULL_HANDLE;
    vkDestroyDevice(device, nullptr);
    return *this;
  }

  void Device::createInstance()
  {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Lava";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers)
    {
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
      createInfo.enabledLayerCount = 0;

    vkCall(vkCreateInstance(&createInfo, nullptr, &instance));
  }

  void Device::createSurface()
  {
    VkWin32SurfaceCreateInfoKHR win32surfaceCreateInfo = {};
    win32surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32surfaceCreateInfo.hwnd = (HWND)hwnd;
    win32surfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);
    vkCall(vkCreateWin32SurfaceKHR(instance, &win32surfaceCreateInfo, nullptr, &surface));
  }

  void Device::pickPhysicalDevice()
  {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    assert(deviceCount != 0); //("failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& currentDevice : devices)
    {
      if (isDeviceSuitable(currentDevice))
      {
        physicalDevice = currentDevice;
        break;
      }
    }
    assert(physicalDevice != VK_NULL_HANDLE); //"failed to find a suitable GPU!"

    swapChainSupportDetails = querySwapChainSupport(physicalDevice);
  }

  Device::QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice _device)
  {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
      if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, surface, &presentSupport);

      if (queueFamily.queueCount > 0 && presentSupport) {
        indices.presentFamily = i;
      }

      if (indices.isComplete()) {
        break;
      }

      i++;
    }

    return indices;
  }

  bool Device::isDeviceSuitable(VkPhysicalDevice _device)
  {
    QueueFamilyIndices indices = findQueueFamilies(_device);

    bool extensionsSupported = checkDeviceExtensionSupport(_device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_device);
      swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(_device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy;
  }

  bool Device::checkDeviceExtensionSupport(VkPhysicalDevice _device)
  {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
      requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
  }

  Device::SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice _device)
  {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(_device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(_device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
  }

  void Device::createLogicalDevice()
  {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo queueCreateInfo = {};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
      createInfo.enabledLayerCount = 0;
    }

    vkCall(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));
      
    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
  }

}
