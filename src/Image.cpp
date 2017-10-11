#pragma once

#include "lava.h"
#include "Debug.h"
#include "Window.h"

namespace lava
{
  class Renderer
  {
  public:
    Renderer() = default;
    virtual ~Renderer();

    void Update();
    void Run()
    {
      while (!mWindow->IsClosed())
        Update();
    }

    void Create();

    Renderer& createWindow(const uint32_t _w, const uint32_t _h)
    {
      mWindow = new Window("Vulkan", _w, _h);
      return *this;
    }

    Renderer& renderingFunction(std::function<void(VkCommandBuffer)> _renderingFnc)
    {
      mRenderingFnc = _renderingFnc;
      return *this;
    }

    Renderer& outputMsgFunction(std::function<void(MessageSeverity, const std::string&)> _outputFnc)
    {
      setOutputMessageFunction(_outputFnc);
      return *this;
    }

    VkDevice GetDevice() const { return mDevice; }
    VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
    VkRenderPass GetRenderPass() const { return mRenderPass; }
    const uint32_t GetFrameBufferWidth() const;
    const uint32_t GetFrameBufferHeight() const;
    const std::vector<MemoryTypeInfo>& GetHeaps() const { return mHeaps; }

  protected:
    VkInstance       mInstance = VK_NULL_HANDLE;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice         mDevice = VK_NULL_HANDLE;
    VkQueue          mQueue = VK_NULL_HANDLE;
    VkSurfaceKHR     mSurface = VK_NULL_HANDLE;
    VkCommandPool    mCommandPool = VK_NULL_HANDLE;
    VkSwapchainKHR   mSwapChain = VK_NULL_HANDLE;
    VkRenderPass     mRenderPass = VK_NULL_HANDLE;

    int              mQueueFamilyIndex = -1;

    VkFence          mFrameFences[sQueueSlotCount];
    VkImage          mImages[sQueueSlotCount];
    VkImageView      mImageViews[sQueueSlotCount];
    VkFramebuffer    mFramebuffer[sQueueSlotCount];

    VkSemaphore      mImageAcquiredSemaphore;
    VkSemaphore      mRenderingCompleteSemaphore;

    Window*          mWindow = nullptr;

    VkCommandBuffer mCommandBuffers[sQueueSlotCount];
    VkCommandBuffer mSetupCommandBuffer;
    uint32_t        mCurrentBackBuffer = 0;

    std::vector<MemoryTypeInfo> mHeaps;

    std::function<void(VkCommandBuffer)> mRenderingFnc = nullptr;

#ifdef _DEBUG
    Debug*           mDebug = nullptr;
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
    void EnumerateHeaps();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
      for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
          return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
          return format;
        }
      }

      errorLog("failed to find supported format!");

      return VkFormat::VK_FORMAT_UNDEFINED;
    }

    VkFormat findDepthFormat()
    {
      return findSupportedFormat(
      { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
      );
    }

    bool hasStencilComponent(VkFormat format)
    {
      return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

  };

}
