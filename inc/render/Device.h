#pragma once

#include "lava.h"
#include <Singleton.hpp>

#include "Buffer.h"
#include "SwapChain.h"

namespace lava
{
  class CCamera;
  class CDevice : public Singleton<CDevice>
  {
  public:
    CDevice() = default;
    virtual ~CDevice() = default;

    void Create( void* hwnd );
    void Destroy();

    VkPhysicalDevice  GetPhyiscalDevice() const { return mPhysicalDevice; }
    VkDevice          GetLogicalDevice()  const { return mDevice; }
    int               GetGraphicsFamily() const { return mQueues.GraphicsFamily; }
    int               GetPresentFamily()  const { return mQueues.PresentFamily; }
    VkCommandPool     GetCommandPool()    const { return mCommandPool; }
    VkQueue           GetGraphicsQueue()  const { return mQueues.Graphics; }
    VkDescriptorPool  GetDescriptorPool() const { return mDescriptorPool; }
    lava::Buffer      GetPerFrameUBO()    const { return mPerFrameUniformBuffer; }
    lava::Buffer      GetLightsUBO()      const { return mLightsUniformBuffer; }
    const CSwapChain& GetSwapChain()      const { return mSwapChain; }

    void BeginFrame(const CCamera& aRenderingCamera);
    uint32_t GetCurrentImageIndex() { return mCurrentImageIndex; }
    VkCommandBuffer GetFrameCommandBuffer() { return mCommandBuffers[mCurrentImageIndex]; }
    void EndFrame();

    VkFormat         FindSupportedFormat(const std::vector<VkFormat>& aCandidates, VkImageTiling aTiling, VkFormatFeatureFlags aFeatures);
    uint32_t         FindMemoryType(uint32_t aTypeFilter, VkMemoryPropertyFlags aProperties);

    VkCommandBuffer BeginSingleExecutionCommand();
    void EndSingleExecutionCommand(VkCommandBuffer aCommandBuffer);

  private:
    
    struct VkQueues
    {
      int GraphicsFamily = -1;
      int PresentFamily = -1;

      bool IsComplete() { return GraphicsFamily >= 0 && PresentFamily >= 0; }

      VkQueue Graphics;
      VkQueue Present;
    };

    VkInstance mInstance = nullptr;
    VkDebugReportCallbackEXT mCallback = nullptr;
    VkPhysicalDevice mPhysicalDevice = nullptr;
    VkDevice         mDevice = nullptr;
    VkCommandPool    mCommandPool = nullptr;
    VkQueues mQueues = {};
    CSwapChain mSwapChain;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkFence> mFences;

    VkDescriptorPool mDescriptorPool;

    uint32_t mCurrentImageIndex = 0;

    VkSemaphore mRenderFinishedSemaphore;


    lava::Buffer mPerFrameUniformBuffer;
    lava::Buffer mLightsUniformBuffer;

    void CreateUniformBuffers();
    void DestroyUniformBuffers();

  private:
    void CreateInstance();
    void SetupDebugCallback();
    void CreateSurface(void* hwnd);
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateDescriptorPool();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateFences();
    void CreateRenderSemaphore();
  };
}
