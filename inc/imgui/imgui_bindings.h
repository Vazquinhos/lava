#pragma once
#include "lava.h"

#include "imgui_impl.h"

namespace lava
{
  void Editor();
  void Menubar();
  void Toolbar();
  void Hiearchy();
  void Inspector();
  
  class Transform;
  void InspectTransform(Transform* _trsf);

  class Camera;
  void InspectCamera(Camera* _camera);

  class Light;
  void ImGuiLight(Light& _light);

  class Texture;
  void ImGuiTexture(Texture& _texture);
}