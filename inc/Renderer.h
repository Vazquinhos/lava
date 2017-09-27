#pragma once

#include "lava.h"

namespace lava
{
    class SwapChain;
    class Window;
    class Debug;
    class Renderer
    {
    public:
        Renderer(Window* _window, const std::vector< std::string >& _requiredExtensions, const std::vector< std::string >& _requiredLayers);
        virtual ~Renderer();

        bool Update();
    
    protected: 
        VkInstance       mInstance          = VK_NULL_HANDLE;
        VkPhysicalDevice mPhysicalDevice    = VK_NULL_HANDLE;
        VkDevice         mDevice            = VK_NULL_HANDLE;
        VkQueue          mQueue             = VK_NULL_HANDLE;
        VkSurfaceKHR     mSurface           = VK_NULL_HANDLE;
        VkCommandPool    mCommandPool       = VK_NULL_HANDLE;
        VkSwapchainKHR   mSwapChain         = VK_NULL_HANDLE;
        VkRenderPass     mRenderPass        = VK_NULL_HANDLE;

        int              mQueueFamilyIndex  = -1;

        VkFence          mFrameFences[sQueueSlotCount];
        VkImage          mImages[sQueueSlotCount];
        VkImageView      mImageViews[sQueueSlotCount];
        VkFramebuffer    mFramebuffer[sQueueSlotCount];

        VkSemaphore      mImageAcquiredSemaphore;
        VkSemaphore      mRenderingCompleteSemaphore;

        Window*          mWindow            = nullptr;

        VkCommandBuffer mCommandBuffers[sQueueSlotCount];
        VkCommandBuffer mSetupCommandBuffer;
        uint32_t        mCurrentBackBuffer = 0;

#ifdef _DEBUG
        Debug*           mDebug             = nullptr;
#endif
    
    protected:
        void CreateInstance(const std::vector<const char*>& _requiredAvailableExtensions, const std::vector< const char* >& _requiredAvailableLayers);
        void CreateDeviceAndQueue();
        void CreateSurface();
        void CreateSwapChain();
        void CreateDebug();
        void CreateCommandPoolAndCommandBuffers();
        void CreateFences();
        void CreateRenderPass(VkFormat _swapchainFormat);
        void CreateSwapchainImageViews(VkFormat _swapchainFormat);
        void CreateFramebuffers();
        void CreateSemaphores();
    };
}
