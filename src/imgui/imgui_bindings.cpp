#include "imgui/imgui_impl.h"
#include "ImGuizmo.h"
#include "imgui/imgui_bindings.h"

#include "graphics/Camera.h"
#include "graphics/Light.h"

#include "ecs/Entity.h"
#include "ecs/World.h"
#include "graphics/Transform.h"

#include "textures/Texture.h"

#include "graphics/visuals/Visual.h"
#include "graphics/visuals/VisualMesh.h"
#include "graphics/AABB.h"

namespace lava
{
  namespace
  {
    static bool sHiearachyOpened = true;
    static bool sInspectorOpened = true;
    static float sWidth = 100.0f;
    static float sMenuBarHeight = 0.0;
    static float sToolBarHeight = 0.0f;
    static size_t sInspectedEntity = 0;
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
  }

  void InspectVisual(Visual* _visual)
  {
    if (ImGui::CollapsingHeader("Visual"))
    {
      if (ImGui::TreeNode("Geometry"))
      {
        for (uint32_t j = 0, count = _visual->geometryCount(); j < count; ++j)
        {
          ImGui::Separator();
          ImGui::Text("Vertices: %d Indices: %d", _visual->geometry(j)->verticesCount(), _visual->geometry(j)->indicesCount() );
        }
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("AABB"))
      {
        glm::vec3 min = _visual->aabb().min();
        glm::vec3 max = _visual->aabb().max();
        ImGui::Float3("Min", min);
        ImGui::Float3("Max", max);
        ImGui::TreePop();
      }
    }
  }

  void Editor()
  {
    sMenuBarHeight = 0.0f;
    Inspector();
    Hiearchy();
  }

  void Menubar()
  {
    ImGui::BeginMainMenuBar();
    ImGui::MenuItem("LAVA ENGINE");
    ImGui::EndMainMenuBar();

    ImGuiContext* context = ImGui::GetCurrentContext();
    sMenuBarHeight = context->FontBaseSize + context->Style.FramePadding.y * 2.0f;
  }

  void Hiearchy()
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, sMenuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y - sToolBarHeight - sMenuBarHeight ));
    ImGui::Begin("Hierarchy", &sHiearachyOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    World& world = lava::World::getInstance();
    const size_t numEntities = world.length();
    for (size_t i = 0; i < numEntities; ++i)
    {
      Entity* currentEntity = world.at(i);

      bool isPressed = sInspectedEntity == i;
      ImGui::BoolButton(currentEntity->name().c_str(), &isPressed);

      if (sInspectedEntity != i && isPressed)
        sInspectedEntity = i;
    }
    ImGui::End();
  }

  void Inspector()
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.75f, sMenuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y - sToolBarHeight - sMenuBarHeight));

    ImGui::Begin("Inspector", &sInspectorOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Separator();

   
    Entity* entity = lava::World::getInstance().at(sInspectedEntity);

    ImGui::Text("Name: %s", entity->name().c_str());
    ImGui::Checkbox("Active", &entity->active());
    const size_t numComponents = entity->length();
    for (size_t i = 0; i < numComponents; ++i)
    {
      Component* currentComponent = entity->at(i);
      switch (currentComponent->type())
      {
      case Component::Type::eTransform:
        InspectTransform(static_cast<Transform*>(currentComponent));
        break;
      case Component::Type::eCamera:
        InspectCamera(static_cast<Camera*>(currentComponent));
        break;
      case Component::Type::eLight:
        InspectLight(static_cast<Light*>(currentComponent));
        break;
      case Component::Type::eVisual:
        InspectVisual(static_cast<Visual*>(currentComponent));
        break;
      }
    }
    ImGui::End();
  }

  void InspectTransform(Transform* _trsf)
  {
    if (ImGui::CollapsingHeader("Transform"))
    {
      World& world = lava::World::getInstance();
      Entity* mainCameraEntity = world.find("main camera");

      if (mainCameraEntity != nullptr)
      {
        Camera* cameraCmp = mainCameraEntity->getComponent<Camera>();
        glm::mat4 view = cameraCmp->view();
        glm::mat4 prj = cameraCmp->proj();
        ImGuiIO& io = ImGui::GetIO();

        bool translation = mCurrentGizmoOperation == ImGuizmo::TRANSLATE;
        ImGui::BoolButton("Translate", &translation);
        if (translation)
          mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();

        bool rotate = mCurrentGizmoOperation == ImGuizmo::ROTATE;
        ImGui::BoolButton("Rotate", &rotate);
        if (rotate)
          mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();

        bool scale = mCurrentGizmoOperation == ImGuizmo::SCALE;
        ImGui::BoolButton("Scale", &scale);
        if (scale)
          mCurrentGizmoOperation = ImGuizmo::SCALE;
        ImGui::SameLine();

        ImGui::Text("|");
        ImGui::SameLine();

        bool world = mCurrentGizmoMode == ImGuizmo::WORLD;
        ImGui::BoolButton("World", &world);
        if (world)
          mCurrentGizmoMode = ImGuizmo::WORLD;
        ImGui::SameLine();

        bool local = mCurrentGizmoMode == ImGuizmo::LOCAL;
        ImGui::BoolButton("Local", &local);
        if (local)
          mCurrentGizmoMode = ImGuizmo::LOCAL;

        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(&view[0][0], &prj[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, _trsf->mat());
        ImGui::Separator();
      }
      _trsf->decompose();
      ImGui::Float3("Position", _trsf->position());
      //ImGui::Float3("Rotation", _trsf->rotation());
      ImGui::Float3("Scale", _trsf->scale());
      _trsf->recompose();
    }
  }

  void InspectCamera(Camera* _camera)
  {
      if (ImGui::CollapsingHeader("Camera"))
      {
        ImGui::EnumCombo("Projection", _camera->mode());
        ImGui::EnumCombo("Clear Mode", _camera->clearMode());

        if (_camera->clearMode() == Camera::ClearMode::eSolidColor)
          ImGui::ColorEdit4("Solid Color", &_camera->clearColor().x);

        ImGui::DragFloat3("Eye", &_camera->eye().x, 0.5f);
        ImGui::DragFloat3("Up", &_camera->up().x, 0.5f);
        ImGui::DragFloat3("Look At", &_camera->lookAt().x, 0.5f);
        
        ImGui::SliderFloat("FOV", &_camera->fov(), 0.0f, 90.0f, "%.0f deg", 1.0f);
        if (ImGui::TreeNode("Clipping Planes"))
        {
          ImGui::PushItemWidth(sWidth-20);
          ImGui::InputFloat("Near", &_camera->nearPlane());
          ImGui::SameLine();
          ImGui::InputFloat("Far", &_camera->farPlane());
          ImGui::PopItemWidth();
          ImGui::TreePop();
        }

        if (ImGui::TreeNode("Viewport"))
        {
          ImGui::PushItemWidth(sWidth);
          ImGui::DragFloat("X##Viewport", &_camera->viewport().x, 0.5f);
          ImGui::SameLine();
          ImGui::DragFloat("Width##Viewport", &_camera->viewport().z, 0.5f);
          ImGui::DragFloat("Y##Viewport", &_camera->viewport().y, 0.5f);
          ImGui::SameLine();
          ImGui::DragFloat("Height##Viewport", &_camera->viewport().w, 0.5f);
          ImGui::PopItemWidth();
          ImGui::TreePop();
        }

        if (GuiMode() == ImGuiMode::eDebug)
        {
          if (ImGui::TreeNode("View"))
          {
            glm::mat4 view = _camera->view();
            ImGui::DragFloat4("Row 0##View", &view[0][0]);
            ImGui::DragFloat4("Row 1##View", &view[1][0]);
            ImGui::DragFloat4("Row 2##View", &view[2][0]);
            ImGui::DragFloat4("Row 3##View", &view[3][0]);
            ImGui::TreePop();
          }
          if (ImGui::TreeNode("Projection"))
          {
            glm::mat4 proj = _camera->proj();
            ImGui::DragFloat4("Row 0##Proj", &proj[0][0]);
            ImGui::DragFloat4("Row 1##Proj", &proj[1][0]);
            ImGui::DragFloat4("Row 2##Proj", &proj[2][0]);
            ImGui::DragFloat4("Row 3##Proj", &proj[3][0]);
            ImGui::TreePop();
          }
        }
      }
  }

  void InspectLight(Light* _light)
  {
    if (ImGui::CollapsingHeader("Light"))
    {
      ImGui::EnumCombo<Light::Type>("Type", _light->type());
      ImGui::ColorEdit3("Color", &_light->color().r);
      ImGui::DragFloat("Intensity", &_light->intensity(), 0.05f, 0.0f, 1.0f);
      ImGui::DragFloat2("Range Attenuation", &_light->rangeAttenuation().x, 0.05f, 0.0f, std::numeric_limits<float>::max() );
    }
  }

  void ImGuiTexture(Texture& _texture)
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Separator();
   
  }

}