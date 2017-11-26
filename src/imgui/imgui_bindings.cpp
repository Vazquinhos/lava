#include "imgui/imgui_impl.h"
#include "ImGuizmo.h"
#include "imgui/imgui_bindings.h"

#include "graphics/Camera.h"
#include "graphics/Light.h"

#include "ecs/Entity.h"
#include "ecs/World.h"
#include "ecs/ComponentTransform.h"

#include "textures/Texture.h"

namespace lava
{
  Entity* HierarchyWorld(World& _world)
  {
    static size_t openIdx = 0;
    const size_t numEntities = _world.length();
    for (size_t i = 0; i < numEntities; ++i)
    {
      Entity* currentEntity = _world.at(i);
      if (ImGui::TreeNode(currentEntity->name().c_str()))
      {
        if (openIdx != i)
          openIdx = i;
        ImGui::TreePop();
      }
    }
    return _world.at(openIdx);
  }

  void InspectEntity(Entity& _entity)
  {
    ImGui::Separator();
    ImGui::Text("Name: %s", _entity.name().c_str());
    ImGui::SameLine();
    ImGui::Checkbox("Active", &_entity.active());
    const size_t numComponents = _entity.length();
    for (size_t i = 0; i < numComponents; ++i)
    {
      Component* currentComponent = _entity.at(i);
      ImGui::Separator();
      switch (currentComponent->type())
      {
      case Component::Type::eTransform:
        InspectTransform(static_cast<Transform*>(currentComponent));
      }
    }
  }

  void InspectTransform(Transform* _trsf)
  {
    if (ImGui::CollapsingHeader("Transform"))
    {
      _trsf->decompose();
      ImGui::DragFloat3("Position", &_trsf->position().x);
      ImGui::DragFloat4("Rotation", &_trsf->rotation().x);
      ImGui::DragFloat3("Scale", &_trsf->scale().x);
    }
  }

  void HierarchyEntity(Entity& _entity)
  {

  }

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

  void ImGuiTexture(Texture& _texture)
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Separator();
    if (_texture.mTextureId != io.Fonts->TexID)
    {
      ImGui::Image(_texture.textureImageMemory, ImVec2(100, 100));
    }
  }

}