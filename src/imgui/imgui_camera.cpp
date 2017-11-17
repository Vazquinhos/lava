#include "imgui/imgui_impl.h"
#include "imgui/imgui_camera.h"

#include "graphics/Camera.h"

namespace lava
{
  void ImGuiCamera(Camera& _camera)
  {
    ImGui::DragFloat3("Eye", &_camera.eye().x);
    ImGui::DragFloat3("Look At", &_camera.lookAt().x);
    if (ImGui::Button("Reset"))
    {
      _camera.eye(glm::vec3(2, 2, 2));
      _camera.lookAt(glm::vec3(0, 0, 0));
    }
  }
}