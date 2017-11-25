#include "imgui/imgui_texture.h"
#include "imgui/imgui_impl.h"

#include "textures/Texture.h"

namespace lava
{
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