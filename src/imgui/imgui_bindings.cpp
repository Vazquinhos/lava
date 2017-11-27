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

  void Editor()
  {
    ImGuiContext* context = ImGui::GetCurrentContext();
    sMenuBarHeight = context->FontBaseSize + context->Style.FramePadding.y * 2.0f;

    Menubar();
    Toolbar();
    Inspector();
    Hiearchy();
  }

  void Menubar()
  {
    ImGui::BeginMainMenuBar();
    ImGui::MenuItem("LAVA ENGINE");
    ImGui::EndMainMenuBar();
  }

  void Toolbar()
  {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - sToolBarHeight));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, sToolBarHeight = sMenuBarHeight * 1.5f));
    ImGui::Begin("ToolBar", false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
    {
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
      ImGui::SameLine();
    }
    ImGui::End();
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
      if (ImGui::TreeNode(currentEntity->name().c_str()))
      {
        if (sInspectedEntity != i)
          sInspectedEntity = i;
        ImGui::TreePop();
      }
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

    World& world = lava::World::getInstance();
    Entity* entity = lava::World::getInstance().at(sInspectedEntity);

    ImGuizmo::Enable(io.WantCaptureMouse == false);

    Entity* mainCameraEntity = world.find("main camera");

    if (mainCameraEntity != nullptr)
    {
      Transform* trsfCmp = entity->getComponent<Transform>();
      if (trsfCmp != nullptr)
      {
        Camera* cameraCmp = mainCameraEntity->getComponent<Camera>();

        cameraCmp->updateMatrices();
        glm::mat4 view = cameraCmp->view();
        glm::mat4 prj = cameraCmp->proj();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        static glm::mat4 trsf(1);
        ImGuizmo::Manipulate(&view[0][0], &prj[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, trsfCmp->mat());
      }
    }

    ImGui::Text("Name: %s", entity->name().c_str());
    ImGui::Checkbox("Active", &entity->active());
    const size_t numComponents = entity->length();
    for (size_t i = 0; i < numComponents; ++i)
    {
      Component* currentComponent = entity->at(i);
      ImGui::Separator();
      switch (currentComponent->type())
      {
      case Component::Type::eTransform:
        InspectTransform(static_cast<Transform*>(currentComponent));
        break;
      case Component::Type::eCamera:
        InspectCamera(static_cast<Camera*>(currentComponent));
        break;
      }
    }
    ImGui::End();
  }

  void InspectTransform(Transform* _trsf)
  {
    if (ImGui::CollapsingHeader("Transform"))
    {
      ImGui::DragFloat3("Position", &_trsf->position().x);
      ImGui::DragFloat3("Rotation", &_trsf->rotation().x);
      ImGui::DragFloat3("Scale", &_trsf->scale().x);
      _trsf->recompose();
    }
  }

  void InspectCamera(Camera* _camera)
  {
      if (ImGui::CollapsingHeader("Camera"))
      {
        ImGui::EnumCombo("Projection", _camera->mode());
        ImGui::DragFloat3("Eye", &_camera->eye().x, 0.5f);
        ImGui::DragFloat3("Up", &_camera->up().x, 0.5f);
        ImGui::DragFloat3("Look At", &_camera->lookAt().x, 0.5f);
        
        ImGui::SliderFloat("FOV", &_camera->fov(), 0.0f, 90.0f, "%.0f deg", 1.0f);
        ImGui::Text("Clipping Planes");
        {
          ImGui::PushItemWidth(sWidth);
          ImGui::InputFloat("Near", &_camera->nearPlane());
          ImGui::SameLine();
          ImGui::PushItemWidth(sWidth);
          ImGui::InputFloat("Far", &_camera->farPlane());
        }
        ImGui::Text("Viewport");
        {
          ImGui::PushItemWidth(sWidth);
          ImGui::DragFloat("X", &_camera->viewport().x, 0.5f);
          ImGui::SameLine();
          ImGui::PushItemWidth(sWidth);
          ImGui::DragFloat("Width", &_camera->viewport().z, 0.5f);
          ImGui::PushItemWidth(sWidth);
          ImGui::DragFloat("Y", &_camera->viewport().y, 0.5f);
          ImGui::SameLine();
          ImGui::PushItemWidth(sWidth);
          ImGui::DragFloat("Height", &_camera->viewport().w, 0.5f);
        }

        if (GuiMode() == ImGuiMode::eDebug)
        {
          if (ImGui::TreeNode("View"))
          {
            glm::mat4 view = _camera->view();
            ImGui::DragFloat4("Row 0", &view[0][0]);
            ImGui::DragFloat4("Row 1", &view[1][0]);
            ImGui::DragFloat4("Row 2", &view[2][0]);
            ImGui::DragFloat4("Row 3", &view[3][0]);
            ImGui::TreePop();
          }
          if (ImGui::TreeNode("Projection"))
          {
            glm::mat4 proj = _camera->proj();
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