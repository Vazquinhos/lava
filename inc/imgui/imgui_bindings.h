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

  class CCamera;
  void InspectCCamera(CCamera* _CCamera);

  class Light;
  void InspectLight(Light* _light);

  class Texture;
  void ImGuiTexture(Texture& _texture);
}