#pragma once
#include "lava.h"

#include <imgui.h>

namespace lava
{
  class Entity;
  class World;
  Entity* HierarchyWorld(World& _world);

  void InspectEntity(Entity& _entity);
  void HierarchyEntity(Entity& _entity);

  class Transform;
  void InspectTransform(Transform* _trsf);

  class Camera;
  void ImGuiCamera(Camera& _camera);

  class Light;
  void ImGuiLight(Light& _light);

  class Texture;
  void ImGuiTexture(Texture& _texture);
}