#include "imgui/imgui_lights.h"
#include "imgui/imgui_impl.h"

#include "graphics/Light.h"

namespace lava
{
  void ImGuiLight(Light& _light)
  {
    if (ImGui::TreeNode("Camera"))
    {
      ImGui::EnumCombo<Light::Type>("Type", _light.type());
      ImGui::TreePop();
    }
    
  }
}