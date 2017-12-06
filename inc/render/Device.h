#pragma once

#include "lava.h"
#include <Singleton.hpp>

#include "SwapChain.h"

namespace lava
{
  class CDevice : public Singleton<CDevice>
  {
  public:
    CDevice() = default;
    virtual ~CDevice() = default;

    void Create( void* hwnd );
    void Destroy();

    VkPhysicalDevice  GetPhyiscalDevice()  const { return mPhysicalDevice; }
    VkDevice          GetLogicalDevice()   const { return mDevice; }
    int               GetGraphicsFamily()  const { return mQueues.GraphicsFamily; }
    int               GetPresentFamily()   const { return mQueues.PresentFamily; }
    VkCommandPool     GetCommandPool()     const { return mCommandPool; }
    VkQueue           GetGraphicsQueue()   const { return mQueues.Graphics; }
    const CSwapChain& GetSwapChain()       const { return mSwapChain; }


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

  private:
    void CreateInstance();
    void SetupDebugCallback();
    void CreateSurface(void* hwnd);
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();
  };
}
