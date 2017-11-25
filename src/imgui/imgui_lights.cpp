#include "imgui/imgui_lights.h"
#include "imgui/imgui_impl.h"

#include "graphics/Light.h"

namespace lava
{
  void ImGuiLight(Light& _light)
  {
    if (ImGui::TreeNode("Light"))
    {
      ImGui::EnumCombo<Light::Type>("Type", _light.type());
      ImGui::DragFloat3("Position", &_light.position().x);
      ImGui::ColorEdit3("Color", &_light.color().r);
      ImGui::TreePop();
    }
  }
}