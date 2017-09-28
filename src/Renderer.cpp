#include "Renderer.h"
#include "Debug.h"
#include "Window.h"

namespace lava
{
    Renderer::Renderer(Window* _window, const std::vector< std::string >& _requiredExtensions, const std::vector< std::string >& _requiredLayers)
        : mWindow(_window)
        , mDebug(nullptr)
    {
        // The extensions asked by the user must be checked with the available extensions in this platform
        uint32_t extensionCount = 0;
        VkResult r = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        assert(r == VK_SUCCESS);

        std::vector<VkExtensionProperties> instanceExtensions{ extensionCount };
        r = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());
        assert(r == VK_SUCCESS);

        // The same goes for the layers asked by the user...
        uint32_t layerCount = 0;
        r = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        assert(r == VK_SUCCESS);

        std::vector<VkLayerProperties> instanceLayers{ layerCount };
        r = vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());
        assert(r == VK_SUCCESS);

        std::vector<const char*> extensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
        std::vector<const char*> layers;

#ifdef  _DEBUG
        const size_t numExtensions = _requiredExtensions.size();
        debugLog("Extensions:");
        for (const auto& e : instanceExtensions)
        {
            if (strcmp(e.extensionName, "VK_EXT_debug_report") == 0)
            {
                extensions.push_back("VK_EXT_debug_report");
                debugLog("VK_EXT_debug_report");
                continue;
            }

            bool extensionFound = false;
            for (size_t iExtension = 0; iExtension < numExtensions && extensionFound == false; ++iExtension)
            {
                if (strcmp(e.extensionName, _requiredExtensions[iExtension].c_str()) == 0)
                {
                    extensionFound = true;
                    extensions.push_back(e.extensionName);
                    debugLog(e.extensionName);
                }
            }
        }

        const size_t numLayers = _requiredLayers.size();
        debugLog("Layers");
        for (const auto& l : instanceLayers)
        {
            if (strcmp(l.layerName, "VK_LAYER_LUNARG_standard_validation") == 0)
            {
                layers.push_back("VK_LAYER_LUNARG_standard_validation");
                debugLog("VK_LAYER_LUNARG_standard_validation");
                continue;
            }

            bool layerFound = false;
            for (size_t iLayer = 0; iLayer < numLayers && layerFound == false; ++iLayer)
            {
                if (strcmp(l.layerName, _requiredLayers[iLayer].c_str()) == 0)
                {
                    layerFound = true;
                    layers.push_back(l.layerName);
                    debugLog(l.layerName);
                }
            }
        }
#else
        if (!_requiredExtensions.empty())
        {
            const size_t numExtensions = _requiredExtensions.size();
            for (const auto& e : instanceExtensions)
            {
                bool extensionFound = false;
                for (size_t iExtension = 0; iExtension < numExtensions && extensionFound == false; ++iExtension)
                {
                    if (strcmp(e.extensionName, _requiredExtensions[iExtension].c_str()) == 0)
                    {
                        extensionFound = true;
                        extensions.push_back(e.extensionName);
                    }
                }
            }
        }

        if (!_requiredLayers.empty())
        {
            const size_t numLayers = _requiredLayers.size();
            for (const auto& l : instanceLayers)
            {
                bool layerFound = false;
                for (size_t iLayer = 0; iLayer < numLayers && layerFound == false; ++iLayer)
                {
                    if (strcmp(l.layerName, _requiredLayers[iLayer].c_str()) == 0)
                    {
                        layerFound = true;
                        layers.push_back(l.layerName);
                    }
                }
            }
        }
#endif
        CreateInstance(extensions, layers);
        CreateDeviceAndQueue();
        CreateSurface();
        CreateSwapChain();
        CreateFramebuffers();
        CreateDebug();
        CreateCommandPoolAndCommandBuffers();
        CreateFences();

        vkResetFences(mDevice, 1, &mFrameFences[0]);
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            vkBeginCommandBuffer(mSetupCommandBuffer, &beginInfo);

            //InitializeImpl(setupCommandBuffer_);

            vkEndCommandBuffer(mSetupCommandBuffer);

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &mSetupCommandBuffer;
            vkQueueSubmit(mQueue, 1, &submitInfo, mFrameFences[0]);
        }

        vkWaitForFences(mDevice, 1, &mFrameFences[0], VK_TRUE, UINT64_MAX);

        CreateSemaphores();
    }

    Renderer::~Renderer()
    {
        // Wait for all rendering to finish
        vkWaitForFences(mDevice, sQueueSlotCount, mFrameFences, VK_TRUE, UINT64_MAX);

        // Destroy semaphores
        vkDestroySemaphore(mDevice, mRenderingCompleteSemaphore, nullptr);
        vkDestroySemaphore(mDevice, mImageAcquiredSemaphore, nullptr);

        // Destroy the fences
        for (int i = 0; i < sQueueSlotCount; ++i)
            vkDestroyFence(mDevice, mFrameFences[i], nullptr);

        // Destroy the command pool
        vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

        // Destroy the rendering pass
        vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

        // Destroy the frame buffer and the image views
        for (int i = 0; i < sQueueSlotCount; ++i)
        {
            vkDestroyFramebuffer(mDevice, mFramebuffer[i], nullptr);
            vkDestroyImageView(mDevice, mImageViews[i], nullptr);
        }

        // Destroy the swap chain and the surface
        vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

#ifdef _DEBUG
        safe_delete(mDebug);
#endif

        // Destroy the instance and the device
        vkDestroyDevice(mDevice, nullptr);
        vkDestroyInstance(mInstance, nullptr);
    }

    void Renderer::Update()
    {
      vkAcquireNextImageKHR(mDevice, mSwapChain, UINT64_MAX, mImageAcquiredSemaphore, VK_NULL_HANDLE, &mCurrentBackBuffer);

      vkWaitForFences(mDevice, 1, &mFrameFences[mCurrentBackBuffer], VK_TRUE, UINT64_MAX);
      vkResetFences(mDevice, 1, &mFrameFences[mCurrentBackBuffer]);

      VkCommandBufferBeginInfo beginInfo = {};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

      vkBeginCommandBuffer(mCommandBuffers[mCurrentBackBuffer], &beginInfo);

      VkRenderPassBeginInfo renderPassBeginInfo = {};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.framebuffer = mFramebuffer[mCurrentBackBuffer];
      renderPassBeginInfo.renderArea.extent.width = mWindow->GetWidth();
      renderPassBeginInfo.renderArea.extent.height = mWindow->GetHeight();
      renderPassBeginInfo.renderPass = mRenderPass;

      VkClearValue clearValue = {};

      clearValue.color.float32[0] = 1.0f;
      clearValue.color.float32[1] = 0.042f;
      clearValue.color.float32[2] = 0.042f;
      clearValue.color.float32[3] = 1.0f;

      renderPassBeginInfo.pClearValues = &clearValue;
      renderPassBeginInfo.clearValueCount = 1;

      vkCmdBeginRenderPass(mCommandBuffers[mCurrentBackBuffer],
        &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

      //RenderImpl(commandBuffers_[currentBackBuffer_]);

      vkCmdEndRenderPass(mCommandBuffers[mCurrentBackBuffer]);
      vkEndCommandBuffer(mCommandBuffers[mCurrentBackBuffer]);

      // Submit rendering work to the graphics queue
      const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      VkSubmitInfo submitInfo = {};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = &mImageAcquiredSemaphore;
      submitInfo.pWaitDstStageMask = &waitDstStageMask;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentBackBuffer];
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &mRenderingCompleteSemaphore;
      vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE);

      // Submit present operation to present queue
      VkPresentInfoKHR presentInfo = {};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = &mRenderingCompleteSemaphore;
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = &mSwapChain;
      presentInfo.pImageIndices = &mCurrentBackBuffer;
      vkQueuePresentKHR(mQueue, &presentInfo);

      vkQueueSubmit(mQueue, 0, nullptr, mFrameFences[mCurrentBackBuffer]);
    }

    void Renderer::CreateInstance(const std::vector<const char*>& _requiredAvailableExtensions, const std::vector< const char* >& _requiredAvailableLayers)
    {
        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.ppEnabledExtensionNames = _requiredAvailableExtensions.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t> (_requiredAvailableExtensions.size());
        instanceCreateInfo.ppEnabledLayerNames = _requiredAvailableLayers.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t> (_requiredAvailableLayers.size());

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pApplicationName = "lava engine app";
        applicationInfo.pEngineName = "lava engine";

        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        VkResult r = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
        assert(r == VK_SUCCESS);

        debugLog("instance ready");
    }

    void Renderer::CreateDeviceAndQueue()
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> devices{ physicalDeviceCount };
        vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, devices.data());

        mQueueFamilyIndex = -1;
        for (auto physicalDevice : devices)
        {
            uint32_t queueFamilyPropertyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilyProperties{ queueFamilyPropertyCount };
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

            for (size_t iQueue = 0, count = queueFamilyProperties.size(); iQueue < count && mPhysicalDevice == VK_NULL_HANDLE; ++iQueue)
            {
                if (queueFamilyProperties[iQueue].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    mPhysicalDevice = physicalDevice;
                    mQueueFamilyIndex = static_cast< uint32_t >( iQueue );
                }
            }
        }

        assert(mPhysicalDevice);
        assert(mQueueFamilyIndex > -1);

        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = mQueueFamilyIndex;

        static const float queuePriorities[] = { 1.0f };
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

        std::vector<const char*> physicalDeviceLayers;

        #ifdef _DEBUG
        uint32_t layerCount = 0;
        vkEnumerateDeviceLayerProperties(mPhysicalDevice, &layerCount, nullptr);

        std::vector<VkLayerProperties> deviceLayers{ layerCount };
        vkEnumerateDeviceLayerProperties(mPhysicalDevice, &layerCount, deviceLayers.data());

        std::vector<const char*> result;
        for (const auto& p : deviceLayers)
        {
            if (strcmp(p.layerName, "VK_LAYER_LUNARG_standard_validation") == 0)
                physicalDeviceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
        }
        #endif

        deviceCreateInfo.ppEnabledLayerNames = physicalDeviceLayers.data();
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t> (physicalDeviceLayers.size());

        std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };

        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t> (deviceExtensions.size());

        vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice);
        assert(mDevice);

        vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mQueue);
        assert(mQueue);

        debugLog("Device and queue ready");
    }

    void Renderer::CreateSurface()
    {
        VkWin32SurfaceCreateInfoKHR win32surfaceCreateInfo = {};
        win32surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        win32surfaceCreateInfo.hwnd = mWindow->GetHWND();
        win32surfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);
        vkCreateWin32SurfaceKHR(mInstance, &win32surfaceCreateInfo, nullptr, &mSurface);
        assert(mSurface);

        VkBool32 presentSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, 0, mSurface, &presentSupported);
        assert(presentSupported);
        debugLog("Surface ready");
    }

    void Renderer::CreateRenderPass(VkFormat _swapchainFormat)
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

    void Renderer::CreateFramebuffers()
    {
        for (int i = 0; i < sQueueSlotCount; ++i)
        {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &mImageViews[i];
            framebufferCreateInfo.height = mWindow->GetHeight();
            framebufferCreateInfo.width = mWindow->GetWidth();
            framebufferCreateInfo.layers = 1;
            framebufferCreateInfo.renderPass = mRenderPass;

            vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mFramebuffer[i]);
        }
    }

    void Renderer::CreateSwapchainImageViews(VkFormat format)
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

    void Renderer::CreateSemaphores()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore);
        vkCreateSemaphore(mDevice, &semaphoreCreateInfo,nullptr, &mRenderingCompleteSemaphore);
    }

    void Renderer::CreateSwapChain()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &surfaceCapabilities);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes{ presentModeCount };
        vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModeCount, presentModes.data());

        VkExtent2D swapChainSize = {};
        swapChainSize = surfaceCapabilities.currentExtent;
        assert(static_cast<int> (swapChainSize.width) == mWindow->GetWidth());
        assert(static_cast<int> (swapChainSize.height) == mWindow->GetHeight());

        uint32_t swapChainImageCount = sQueueSlotCount;
        assert(swapChainImageCount >= surfaceCapabilities.minImageCount);

        // 0 indicates unlimited number of images
        if (surfaceCapabilities.maxImageCount != 0)
            assert(swapChainImageCount < surfaceCapabilities.maxImageCount);

        VkSurfaceTransformFlagBitsKHR surfaceTransformFlags = (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ?
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats{ surfaceFormatCount };
        vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatCount, surfaceFormats.data());

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = mSurface;
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

        vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo, nullptr, &mSwapChain);
        assert(mSwapChain);

        CreateRenderPass(swapchainCreateInfo.imageFormat);
        CreateSwapchainImageViews(swapchainCreateInfo.imageFormat);

        debugLog("Swap chain ready");
    }

    void Renderer::CreateDebug()
    {
#ifdef _DEBUG
        mDebug = new Debug(mInstance);
        debugLog("Debug interface ready");
#endif
    }

    void Renderer::CreateCommandPoolAndCommandBuffers()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = mQueueFamilyIndex;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool);

        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandBufferCount = sQueueSlotCount + 1;
        commandBufferAllocateInfo.commandPool = mCommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkCommandBuffer commandBuffers[sQueueSlotCount + 1];
        vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, commandBuffers);
        for (int i = 0; i < sQueueSlotCount; ++i)
            mCommandBuffers[i] = commandBuffers[i];

        mSetupCommandBuffer = commandBuffers[sQueueSlotCount];
    }

    void Renderer::CreateFences()
    {
        for (int i = 0; i < sQueueSlotCount; ++i)
        {
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            // We need this so we can wait for them on the first try
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mFrameFences[i]);
        }
    }
}
