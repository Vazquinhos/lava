#include "imgui/imgui_impl.h"
#include "imgui/imgui_camera.h"

#include "graphics/Camera.h"

#include <cmath>
#include <limits>
#include <algorithm>

namespace lava
{
  void ImGuiCamera(Camera& _camera)
  {
      static bool sTreeOpened = true;
      ImGui::SetNextTreeNodeOpen(sTreeOpened);
      if (sTreeOpened = ImGui::TreeNode("Camera"))
      {
        ImGui::DragFloat3("Eye", &_camera.eye().x, 0.5f);
        ImGui::DragFloat3("Up", &_camera.up().x, 0.5f);
        ImGui::DragFloat3("Look At", &_camera.lookAt().x, 0.5f);
        
        ImGui::SliderFloat("FOV", &_camera.fov(), 0.0f, 90.0f, "%.0f deg", 1.0f);
        ImGui::Text("Clipping Planes");
        {
          ImGui::Indent();
          ImGui::InputFloat("Near", &_camera.nearPlane(), 0.1f);
          ImGui::InputFloat("Far", &_camera.farPlane(), 1.0f);
          ImGui::Unindent();
        }
        ImGui::TreePop();

        if (ImGui::TreeNode("View"))
        {
          glm::mat4 view = _camera.view();
          ImGui::DragFloat4("Row 0", &view[0][0]);
          ImGui::DragFloat4("Row 1", &view[1][0]);
          ImGui::DragFloat4("Row 2", &view[2][0]);
          ImGui::DragFloat4("Row 3", &view[3][0]);
          ImGui::TreePop();
        }
      }
      ImGui::Separator();
  }
}