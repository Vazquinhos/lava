#pragma once

#include "Renderer.h"

namespace lava
{
    class SwapChain
    {
    public:
        SwapChain(VkSurfaceKHR _surface, uint32_t _surfaceWidth, uint32_t _surfaceHeight, VkDevice _device, VkPhysicalDevice _physicalDevice);
        virtual ~SwapChain();

        static int GetQueueSlotCount()
        {
            return sQueueSlotCount;
        }

    protected:
        static const int sQueueSlotCount = 3;

        uint32_t         mWidth  = 600u;
        uint32_t         mHeight = 800u;
        VkSwapchainKHR   mSwapChain = VK_NULL_HANDLE;
        VkDevice         mDevice    = VK_NULL_HANDLE;
        VkRenderPass     mRenderPass = VK_NULL_HANDLE;
        VkFence          mFrameFences_[sQueueSlotCount];
        VkImage          mImages[sQueueSlotCount];
        VkImageView      mImageViews[sQueueSlotCount];
        VkFramebuffer    mFramebuffer[sQueueSlotCount];

    protected:
        void CreateRenderPass(VkFormat _swapchainFormat);
        void CreateSwapchainImageViews(VkFormat _swapchainFormat);
        void CreateFramebuffers();
    };
}
