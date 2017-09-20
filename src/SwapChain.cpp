#include "SwapChain.h"
#include "Debug.h"

namespace lava
{
    SwapChain::SwapChain(VkSurfaceKHR _surface, uint32_t _surfaceWidth, uint32_t _surfaceHeight, VkDevice _device, VkPhysicalDevice _physicalDevice)
        : mDevice(_device)
        , mHeight(_surfaceHeight)
        , mWidth(_surfaceWidth)
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &surfaceCapabilities);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes{ presentModeCount };
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, presentModes.data());

        VkExtent2D swapChainSize = {};
        swapChainSize = surfaceCapabilities.currentExtent;
        assert(static_cast<int> (swapChainSize.width) == _surfaceWidth);
        assert(static_cast<int> (swapChainSize.height) == _surfaceHeight);

        uint32_t swapChainImageCount = sQueueSlotCount;
        assert(swapChainImageCount >= surfaceCapabilities.minImageCount);

        // 0 indicates unlimited number of images
        if (surfaceCapabilities.maxImageCount != 0)
            assert(swapChainImageCount < surfaceCapabilities.maxImageCount);

        VkSurfaceTransformFlagBitsKHR surfaceTransformFlags = (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ?
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &surfaceFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats{ surfaceFormatCount };
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &surfaceFormatCount, surfaceFormats.data());

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = _surface;
        swapchainCreateInfo.minImageCount = swapChainImageCount;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = surfaceTransformFlags;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.imageExtent = swapChainSize;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.imageColorSpace = surfaceFormats.front().colorSpace;

        if (surfaceFormatCount == 1 && surfaceFormats.front().format == VK_FORMAT_UNDEFINED)
        {
            swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
            warningLog("swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM because undefined format");
        }
        else
        {
            swapchainCreateInfo.imageFormat = surfaceFormats.front().format;
        }

        vkCreateSwapchainKHR(_device, &swapchainCreateInfo, nullptr, &mSwapChain);
        assert(mSwapChain);

        CreateRenderPass(swapchainCreateInfo.imageFormat);
        CreateSwapchainImageViews(swapchainCreateInfo.imageFormat);
        CreateFramebuffers();
    }

    SwapChain::~SwapChain()
    {
        vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

        for (int i = 0; i < sQueueSlotCount; ++i)
        {
            vkDestroyFramebuffer(mDevice, mFramebuffer[i], nullptr);
            vkDestroyImageView(mDevice, mImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
    }

    void SwapChain::CreateRenderPass(VkFormat _swapchainFormat)
    {
        VkAttachmentDescription attachmentDescription = {};
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.format = _swapchainFormat;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkAttachmentReference attachmentReference = {};
        attachmentReference.attachment = 0;
        attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pColorAttachments = &attachmentReference;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.pAttachments = &attachmentDescription;

        vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
        assert(mRenderPass);
    }

    void SwapChain::CreateFramebuffers()
    {
        for (int i = 0; i < sQueueSlotCount; ++i)
        {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &mImageViews[i];
            framebufferCreateInfo.height = mHeight;
            framebufferCreateInfo.width = mWidth;
            framebufferCreateInfo.layers = 1;
            framebufferCreateInfo.renderPass = mRenderPass;

            vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mFramebuffer[i]);
        }
    }

    void SwapChain::CreateSwapchainImageViews(VkFormat format )
    {
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(mDevice, mSwapChain, &swapchainImageCount, nullptr);
        assert(static_cast<int> (swapchainImageCount) == sQueueSlotCount);

        vkGetSwapchainImagesKHR(mDevice, mSwapChain, &swapchainImageCount, mImages);

        for (int i = 0; i < sQueueSlotCount; ++i)
        {
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = mImages[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = format;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

            vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mImageViews[i]);
        }
    }

}
