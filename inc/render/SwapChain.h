#pragma once

#include "lava.h"

#include "render/Image.h"

namespace lava
{
  class CSwapChain
  {
  public:
    struct Details
    {
      VkSurfaceCapabilitiesKHR Capabilities;
      std::vector<VkSurfaceFormatKHR> Formats;
      std::vector<VkPresentModeKHR> PresentModes;
    };

  public:
    CSwapChain() = default;
    virtual ~CSwapChain() = default;

    void Create();
    void Destroy();
    bool IsComplete();
    void SetPhysicalDevice(VkPhysicalDevice aPhysicalDevice);

    VkRenderPass GetRenderPass() const { return mRenderPass; }
    VkExtent2D GetExtent2D() const { return mDetails.Capabilities.currentExtent; }

  private:
    friend class CDevice;

    Details mDetails;
    VkSurfaceKHR mSurface = nullptr;
    VkSurfaceFormatKHR mSurfaceFormat = {};
    VkPresentModeKHR mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D mExtent = {};
    VkSwapchainKHR mSwapChain = nullptr;
    VkRenderPass mRenderPass;

    std::vector<VkImage>       mImages;
    std::vector<VkImageView>   mImageViews;
    std::vector<VkFramebuffer> mFramebuffers;

    CImage mDepthImage;
    
    void ChooseSurfaceFormat();
    void ChoosePresentMode();
    void ChooseExtent();
    void CreateRenderPass();
    void CreateFrameBuffers();
    void CreateImageViews();
    void CreateDepth();
  };
}
