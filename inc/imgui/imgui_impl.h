// ImGui GLFW binding with Vulkan + shaders
// FIXME: Changes of ImTextureID aren't supported by this binding! Please, someone add it!

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 5 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXX_CreateFontsTexture(), ImGui_ImplXXXX_NewFrame(), ImGui_ImplXXXX_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "lava.h"

#include <EnumStringConversor.hpp>

#include <imgui.h>

namespace lava
{
  class Window;

#define IMGUI_VK_QUEUED_FRAMES 2

  struct ImGuiInit_Data
  {
    VkAllocationCallbacks* allocator;
    VkPhysicalDevice       gpu;
    VkDevice               device;
    VkRenderPass           render_pass;
    VkPipelineCache        pipeline_cache;
    void(*check_vk_result)(VkResult err);
  };

  IMGUI_API bool        ImGuiInit(Window* window, ImGuiInit_Data *init_data);
  IMGUI_API void        ImGuiShutdown();
  IMGUI_API void        ImGuiNewFrame();
  IMGUI_API void        ImGuiRender(VkCommandBuffer command_buffer);

  // Use if you want to reset your rendering device without losing ImGui state.
  IMGUI_API void        ImGuiInvalidateFontUploadObjects();
  IMGUI_API void        ImGuiInvalidateDeviceObjects();
  IMGUI_API bool        ImGuiCreateFontsTexture(VkCommandBuffer command_buffer);
  IMGUI_API bool        ImGuiCreateDeviceObjects();

  enum class ImGuiMode
  {
    eNormal = 0,
    eDebug,
  };

  ImGuiMode& GuiMode();
}

namespace ImGui
{
  template< typename EnumType >
  void EnumCombo(const char* _label, EnumType& _type)
  {
    static int sMaxItems = static_cast<int>(EnumType::MAX);
    static bool sItemsFilled = false;
    static const char* sItems[EnumType::MAX] = {};
    if (sItemsFilled == false)
    {
      sItemsFilled = true;
      for (int i(0); i < sMaxItems; ++i)
        sItems[i] = EnumString<EnumType>::asString(static_cast<EnumType>(i)).c_str();
    }
    
    int idx = static_cast<int>(_type);
    ImGui::Combo(_label, &idx, sItems, sMaxItems);
    _type = static_cast<EnumType>(idx);
  }
}