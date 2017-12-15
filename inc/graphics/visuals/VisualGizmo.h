#pragma once

#include "lava.h"

namespace lava
{
  class CCamera;
  class AABB;
  namespace Gizmo
  {
    void NewFrame();
    void BoundingBox(const CCamera& _CCamera, const AABB& _aabb, const glm::mat4& _modelMatrix, const glm::vec4& _color = glm::vec4(255,255,0,150));
    void Cone(const CCamera& _CCamera, const glm::vec3& _basePosition, const glm::vec3& _direction, const glm::vec3& _up, float _baseRadius, float _topRadius, float _height, const glm::vec4& _color = glm::vec4(0, 255, 0, 150));
  }
}
