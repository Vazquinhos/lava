#include "graphics/visuals/VisualGizmo.h"

#include "graphics/Camera.h"
#include "graphics/AABB.h"

#include "imgui/imgui_impl.h"

namespace lava
{
  namespace Gizmo
  {
    namespace
    {
      static float sGuizmoLineThickness = 2.0f;
      ImDrawList* sDrawList = nullptr;
    }
    void NewFrame()
    {
      ImGuiIO& io = ImGui::GetIO();
      ImGui::Begin("lavaGizmo", NULL, io.DisplaySize, 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
      sDrawList = ImGui::GetWindowDrawList();
      ImGui::End();
    }

    /*
    void BoundingBox(const CCamera& _camera, const AABB& _aabb, const glm::mat4& _modelMatrix, const glm::vec4& _color)
    {
      lavaAssert(sDrawList, "Invalid draw list");

      lava::AABB transformedAABB = _aabb.transformed(_modelMatrix);
      const std::vector< glm::vec3 > corners = transformedAABB.corners();
      
      std::vector<glm::vec2> corners2d;
      for (const glm::vec3& c : corners)
        corners2d.push_back(_camera.worldToScreenCoordinates(c));

      ImU32 color = IM_COL32(_color.r, _color.g, _color.b, _color.a);

      static_assert(sizeof(ImVec2) == sizeof(glm::vec2));

      glm::vec2 spoint = _camera.worldToScreenCoordinates(glm::vec3(0));
      
      // Bottom
      sDrawList->AddCircleFilled(ImVec2(_camera.worldToScreenCoordinates(glm::vec3(0)).x, _camera.worldToScreenCoordinates(glm::vec3(0)).y), 10.0f, 64);

      //sDrawList->AddPolyline(reinterpret_cast<ImVec2*>(&corners2d[0]), 4, color, true, sGuizmoLineThickness, true);

      // Top
      //sDrawList->AddPolyline(reinterpret_cast<ImVec2*>(&corners2d[4]), 4, color, true, sGuizmoLineThickness, true);

      // Down lines
      const ImVec2* points2D = reinterpret_cast<ImVec2*>(corners2d.data());
      //sDrawList->AddLine(points2D[0], points2D[6], color, sGuizmoLineThickness);
     // sDrawList->AddLine(points2D[1], points2D[7], color, sGuizmoLineThickness);
      //sDrawList->AddLine(points2D[2], points2D[4], color, sGuizmoLineThickness);
      //sDrawList->AddLine(points2D[3], points2D[5], color, sGuizmoLineThickness);
    }

    void Cone(const Camera& _camera, const glm::vec3& _basePosition, const glm::vec3& _direction, const glm::vec3& _up, float _baseRadius, float _topRadius, float _height, const glm::vec4& _color)
    {
      lavaAssert(sDrawList, "Invalid draw list");

      ImU32 color = IM_COL32(_color.r, _color.g, _color.b, _color.a);

      static uint32_t numSegments = 20u;
      const float step = 360 / numSegments;

      glm::vec3 coversVector = _up;
      glm::quat quat = glm::angleAxis(0.0f, _direction);
      std::vector< ImVec2 > positionsBase;
      std::vector< ImVec2 > positionsTop;
      _baseRadius = std::max(_baseRadius, 0.0f);
      _topRadius = std::max(_topRadius, 0.0f);
      for (float i = 0; i <= 360; i += step)
      {
        glm::vec2 posBase = _camera.worldToScreenCoordinates(_basePosition + coversVector * _baseRadius);
        positionsBase.push_back(ImVec2(posBase.x, posBase.y));

        glm::vec2 posTop = _camera.worldToScreenCoordinates(( _basePosition + _direction * _height) + coversVector * _topRadius);
        positionsTop.push_back(ImVec2(posTop.x, posTop.y));

        coversVector = glm::angleAxis(glm::radians(i), _direction) * _up;

        if ( static_cast<int>(std::floor(i)) % 4 == 0)
          sDrawList->AddLine(ImVec2(posBase.x, posBase.y), ImVec2(posTop.x, posTop.y), color, sGuizmoLineThickness);
      }
      
      sDrawList->AddPolyline(positionsBase.data(), positionsBase.size(), color, true, sGuizmoLineThickness, true);
      sDrawList->AddPolyline(positionsTop.data(), positionsTop.size(), color, true, sGuizmoLineThickness, true);
    }
    */
  }
}
