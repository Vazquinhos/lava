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
      if (ImGui::TreeNode("Camera"))
      {
        ImGui::EnumCombo("Projection Mode", _camera.mode());
        ImGui::DragFloat3("Eye", &_camera.eye().x, 0.5f);
        ImGui::DragFloat3("Up", &_camera.up().x, 0.5f);
        ImGui::DragFloat3("Look At", &_camera.lookAt().x, 0.5f);
        
        ImGui::SliderFloat("FOV", &_camera.fov(), 0.0f, 90.0f, "%.0f deg", 1.0f);
        ImGui::Text("Clipping Planes");
        {
          ImGui::Indent();
          ImGui::InputFloat("Near", &_camera.nearPlane());
          ImGui::InputFloat("Far", &_camera.farPlane());
          ImGui::Unindent();
        }
        ImGui::TreePop();

        ImGui::InputFloat4("Viewport", &_camera.viewport().x);
        

        if (GuiMode() == ImGuiMode::eDebug)
        {
          if (ImGui::TreeNode("View"))
          {
            glm::mat4 view = _camera.view();
            ImGui::DragFloat4("Row 0", &view[0][0]);
            ImGui::DragFloat4("Row 1", &view[1][0]);
            ImGui::DragFloat4("Row 2", &view[2][0]);
            ImGui::DragFloat4("Row 3", &view[3][0]);
            ImGui::TreePop();
          }
          if (ImGui::TreeNode("Projection"))
          {
            glm::mat4 proj = _camera.proj();
            ImGui::DragFloat4("Row 0", &proj[0][0]);
            ImGui::DragFloat4("Row 1", &proj[1][0]);
            ImGui::DragFloat4("Row 2", &proj[2][0]);
            ImGui::DragFloat4("Row 3", &proj[3][0]);
            ImGui::TreePop();
          }
        }
      }
  }
}