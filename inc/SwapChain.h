#pragma once

#include "lava.h"

namespace lava
{
  class SwapChain
  {
  public:
	SwapChain() = default;
	virtual ~SwapChain() = default;

	SwapChain& create();
	SwapChain& destroy();

  SwapChain& windowSize(VkExtent2D _extend) { windowExtent = _extend; return *this; };
  const std::vector<VkFramebuffer>& frameBuffers() const { return swapChainFramebuffers; }

  VkSwapchainKHR swapchainKHR() const { return swapChain; }
  VkRenderPass   renderPass() const { return rnderPass; };
  VkExtent2D     extent() const { return swapChainExtent;};

  protected:
    VkRenderPass rnderPass;
	  VkSwapchainKHR swapChain;
	  std::vector<VkImage> swapChainImages;
	  VkFormat swapChainImageFormat;
    VkExtent2D windowExtent;
	  VkExtent2D swapChainExtent;
	  std::vector<VkImageView> swapChainImageViews;
	  std::vector<VkFramebuffer> swapChainFramebuffers;

    VkFormat depthFormat;
    struct
    {
      VkImage image;
      VkDeviceMemory mem;
      VkImageView view;
    } depthStencil;

	  void createSwapChain();
    void createFramebuffers();
	  void createImageViews();
    void createRenderPass();
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createDepthStencil();

  };

}
