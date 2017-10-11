#pragma once

#include "lava.h"

namespace lava
{
  class Device
  {
  public:
    Device() = default;
    virtual ~Device() = default;

    Device& create();
    Device& destroy();
    Device& enableValidation(bool _enable) { enableValidationLayers = _enable; return *this; }
    Device& windowHandle(void* _handle) { hwnd = _handle; return *this; }

    const std::vector<VkSurfaceFormatKHR>& surfaceFormats() const { return swapChainSupportDetails.formats; }
    const std::vector<VkPresentModeKHR>& presentModes() const { return swapChainSupportDetails.presentModes; }

  protected:
    VkInstance       instance       = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice         device         = VK_NULL_HANDLE;
    VkSurfaceKHR     surface        = VK_NULL_HANDLE;
    VkQueue          graphicsQueue;
    VkQueue          presentQueue;
    void*            hwnd           = nullptr;
    bool             enableValidationLayers = false;

    struct SwapChainSupportDetails
    {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR>   presentModes;
    };

    SwapChainSupportDetails swapChainSupportDetails;
    
    struct QueueFamilyIndices
    {
      int graphicsFamily = -1;
      int presentFamily = -1;
      bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
    };

    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    // Functions to find the best physical device
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  };

}
