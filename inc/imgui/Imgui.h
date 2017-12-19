#pragma once

#include "lava.h"

#include <EnumStringConversor.hpp>

struct ImDrawData;

namespace lava
{
  class Window;
  class CImGui
  {
  public:
    CImGui() = default;
    virtual ~CImGui() = default;

    void Create(Window* aWindow);
    void NewFrame();
    void Render();
    void Destroy();

    void SetStyle();
    
  private:

    VkDescriptorPool       mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout  mDescriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout       mPipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet        mDescriptorSet = VK_NULL_HANDLE;
    VkPipeline             mPipeline = VK_NULL_HANDLE;

    VkSampler              mSampler;
    CImagePtr              mImage;
    ShaderPtr              mVertexShader;
    ShaderPtr              mFragmentShader;

    Window*                mWindow = nullptr;

    void DrawLists(ImDrawData* draw_data);
  };
}