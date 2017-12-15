#include "render/SwapChain.h"

#include <Logger.hpp>

#include "render/Device.h"

namespace lava
{
  void CSwapChain::SetPhysicalDevice(VkPhysicalDevice aPhysicalDevice)
  {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(aPhysicalDevice, mSurface, &mDetails.Capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(aPhysicalDevice, mSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
      mDetails.Formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(aPhysicalDevice, mSurface, &formatCount, mDetails.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(aPhysicalDevice, mSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
      mDetails.PresentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(aPhysicalDevice, mSurface, &presentModeCount, mDetails.PresentModes.data());
    }
  }

  uint32_t CSwapChain::GetNextImage()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    uint32_t lImageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(lLogicalDevice, mSwapChain, std::numeric_limits<uint64_t>::max(), mImageAvailableSemaphore, VK_NULL_HANDLE, &lImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      vkDeviceWaitIdle(lLogicalDevice);
      Destroy();
      Create();
      return lImageIndex;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    return lImageIndex;
  }

  void CSwapChain::ChooseSurfaceFormat()
  {
    if (mDetails.Formats.size() == 1 && mDetails.Formats[0].format == VK_FORMAT_UNDEFINED)
    {
      mSurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }
    else
    {
      for (const auto& lCurrentFormat : mDetails.Formats)
      {
        if (lCurrentFormat.format == VK_FORMAT_B8G8R8A8_UNORM && lCurrentFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
          mSurfaceFormat = lCurrentFormat;
          return;
        }
      }

      mSurfaceFormat = mDetails.Formats[0];
    }
  }

  void CSwapChain::ChoosePresentMode()
  {
    for (const auto& lCurrentPresentMode : mDetails.PresentModes)
    {
      if (lCurrentPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      {
        mPresentMode = lCurrentPresentMode;
        return;
      }
      else if (lCurrentPresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
      {
        mPresentMode = lCurrentPresentMode;
        return;
      }
    }
  }

  void CSwapChain::ChooseExtent()
  {
    lavaAssert(mDetails.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max(), "The extend is invalid");
    mExtent =  mDetails.Capabilities.currentExtent;
  }

  void CSwapChain::Create()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    lavaAssert(IsComplete(), "the swap chain is not complete");

    ChooseSurfaceFormat();
    ChoosePresentMode();
    ChooseExtent();

    uint32_t lImageCount = mDetails.Capabilities.minImageCount + 1;
    if (mDetails.Capabilities.maxImageCount > 0 && lImageCount > mDetails.Capabilities.maxImageCount)
      lImageCount = mDetails.Capabilities.maxImageCount;

    vkNew(VkSwapchainCreateInfoKHR, VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, lCreateInfo);
    
    lCreateInfo.surface = mSurface;

    lCreateInfo.minImageCount    = lImageCount;
    lCreateInfo.imageFormat      = mSurfaceFormat.format;
    lCreateInfo.imageColorSpace  = mSurfaceFormat.colorSpace;
    lCreateInfo.imageExtent      = mExtent;
    lCreateInfo.imageArrayLayers = 1;
    lCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t lQueueFamilyIndices[] = 
    { 
      static_cast<uint32_t>(lDevice.GetGraphicsFamily()),
      static_cast<uint32_t>(lDevice.GetPresentFamily())
    };

    lCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (lQueueFamilyIndices[0] != lQueueFamilyIndices[1])
    {
      lCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      lCreateInfo.queueFamilyIndexCount = 2;
      lCreateInfo.pQueueFamilyIndices = lQueueFamilyIndices;
    }

    lCreateInfo.preTransform = mDetails.Capabilities.currentTransform;
    lCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    lCreateInfo.presentMode = mPresentMode;
    lCreateInfo.clipped = VK_TRUE;

    vkCall(vkCreateSwapchainKHR(lLogicalDevice, &lCreateInfo, nullptr, &mSwapChain));

    // Get the swap chain images
    vkCall(vkGetSwapchainImagesKHR(lLogicalDevice, mSwapChain, &lImageCount, nullptr));
    mImages.resize(lImageCount);
    vkGetSwapchainImagesKHR(lLogicalDevice, mSwapChain, &lImageCount, mImages.data());

    CreateImageViews();
    CreateRenderPass();
    CreateDepth();
    CreateFrameBuffers();
    CreateAcquireImageSemaphore();
  }

  void CSwapChain::CreateRenderPass()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    // Configure color attachment
    VkAttachmentDescription lColorAttachment = {};
    lColorAttachment.format = mSurfaceFormat.format;
    lColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    lColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    lColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    lColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    lColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    lColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    lColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference lColorAttachmentRef = {};
    lColorAttachmentRef.attachment = 0;
    lColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Configure depth attachment
    VkAttachmentDescription lDepthAttachment = {};
    lDepthAttachment.format = lDevice.FindSupportedFormat
    (
      { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
      VK_IMAGE_TILING_OPTIMAL, 
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
    lDepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    lDepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    lDepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    lDepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    lDepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    lDepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    lDepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference lDepthAttachmentRef = {};
    lDepthAttachmentRef.attachment = 1;
    lDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription lSubpass = {};
    lSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    lSubpass.colorAttachmentCount = 1;
    lSubpass.pColorAttachments = &lColorAttachmentRef;
    lSubpass.pDepthStencilAttachment = &lDepthAttachmentRef;

    VkSubpassDependency lDependency = {};
    lDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    lDependency.dstSubpass = 0;
    lDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    lDependency.srcAccessMask = 0;
    lDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    lDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> lAttachments = { lColorAttachment, lDepthAttachment };
    VkRenderPassCreateInfo lRenderPassInfo = {};
    lRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    lRenderPassInfo.attachmentCount = static_cast<uint32_t>(lAttachments.size());
    lRenderPassInfo.pAttachments = lAttachments.data();
    lRenderPassInfo.subpassCount = 1;
    lRenderPassInfo.pSubpasses = &lSubpass;
    lRenderPassInfo.dependencyCount = 1;
    lRenderPassInfo.pDependencies = &lDependency;

    vkCall(vkCreateRenderPass(lLogicalDevice, &lRenderPassInfo, nullptr, &mRenderPass));
  }

  void CSwapChain::CreateFrameBuffers()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    mFramebuffers.resize(mImageViews.size());

    for (size_t i = 0, lCount = mImageViews.size(); i < lCount; ++i)
    {
      std::array<VkImageView, 2> lAttachments = { mImageViews[i], mDepthImage.GetView() };

      vkNew(VkFramebufferCreateInfo, VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, lCreateInfo);

      lCreateInfo.renderPass = mRenderPass;
      lCreateInfo.attachmentCount = static_cast<uint32_t>(lAttachments.size());
      lCreateInfo.pAttachments = lAttachments.data();
      lCreateInfo.width = mDetails.Capabilities.currentExtent.width;
      lCreateInfo.height = mDetails.Capabilities.currentExtent.height;
      lCreateInfo.layers = 1;

      vkCall(vkCreateFramebuffer(lLogicalDevice, &lCreateInfo, nullptr, &mFramebuffers[i]));
    }
  }

  void CSwapChain::CreateImageViews()
  {
    mImageViews.resize(mImages.size());

    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    for (uint32_t i = 0; i < mImages.size(); ++i)
    {
      vkNew(VkImageViewCreateInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, lCreateInfo)

      lCreateInfo.image = mImages[i];
      lCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      lCreateInfo.format = mSurfaceFormat.format;
      lCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      lCreateInfo.subresourceRange.baseMipLevel = 0;
      lCreateInfo.subresourceRange.levelCount = 1;
      lCreateInfo.subresourceRange.baseArrayLayer = 0;
      lCreateInfo.subresourceRange.layerCount = 1;

      vkCall(vkCreateImageView(lLogicalDevice, &lCreateInfo, nullptr, &mImageViews[i]));
    }
  }

  void CSwapChain::CreateDepth()
  {
    CDevice& lDevice = CDevice::getInstance();

    VkFormat lDepthFormat = lDevice.FindSupportedFormat
    (
      { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );

    mDepthImage.Create(mDetails.Capabilities.currentExtent.width, mDetails.Capabilities.currentExtent.height, lDepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mDepthImage.Transition(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }

  void CSwapChain::Destroy()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    vkDestroySemaphore(lLogicalDevice, mImageAvailableSemaphore, nullptr);

    mDepthImage.Destroy();

    for( VkFramebuffer& lCurrentFrameBuffer : mFramebuffers )
      vkDestroyFramebuffer(lLogicalDevice, lCurrentFrameBuffer, nullptr);

    vkDestroyRenderPass(lLogicalDevice, mRenderPass, nullptr);

    for(VkImageView& lCurrentView : mImageViews)
      vkDestroyImageView(lLogicalDevice, lCurrentView, nullptr);

    vkDestroySwapchainKHR(lLogicalDevice, mSwapChain, nullptr);
  }

  bool CSwapChain::IsComplete()
  {
    return !mDetails.Formats.empty() && !mDetails.PresentModes.empty();
  }

  void CSwapChain::CreateAcquireImageSemaphore()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();
    vkNew(VkSemaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, lCreateInfo);
    vkCall(vkCreateSemaphore(lLogicalDevice, &lCreateInfo, nullptr, &mImageAvailableSemaphore));
  }
}
