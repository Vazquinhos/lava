#include "imgui/imgui_impl.h"
#include "imgui/imgui_camera.h"

#include "graphics/Camera.h"

#include <cmath>
#include <limits>
#include <algorithm>

namespace lava
{
  namespace
  {
    struct Fraction
    {
      int WholePart;
      int Numerator;
      int Denominator;
    };

    //Precision controls how many digits after the decimal are kept
    Fraction DecimalToFraction(float DecimalNum)
    {
      Fraction f = {};
      const int MaxIntDigits = std::numeric_limits<int>::digits10;
      const int WholeDigits = int(log10(DecimalNum));
      const int FractionDigits = std::min(std::numeric_limits<float>::digits10 - WholeDigits, MaxIntDigits - 1);

      //If number has too many digits, can't convert
      if (WholeDigits > MaxIntDigits)
      {
        return f;
      }

      //Separate into whole part and fraction
      float WholePart;
      DecimalNum = modf(DecimalNum, &WholePart);
      f.WholePart = int(WholePart);

      //Convert the decimal to a fraction
      const float Denominator = pow(10.0, FractionDigits);
      f.Numerator = int((DecimalNum * Denominator) + 0.5);
      f.Denominator = int(Denominator);

      //Return success
      return f;
    }

    float FractionToDecimal(Fraction &FractionNum)
    {
      const double WholePart = double(FractionNum.WholePart);
      const double DecimalPart = double(FractionNum.Numerator) / double(FractionNum.Denominator);
      return WholePart + DecimalPart;
    }
  }

  void ImGuiCamera(Camera& _camera)
  {
      static bool sTreeOpened = true;
      ImGui::SetNextTreeNodeOpen(sTreeOpened);
      if (sTreeOpened = ImGui::TreeNode("Camera"))
      {
        ImGui::DragFloat3("Eye", &_camera.eye().x, 0.5f);
        ImGui::DragFloat3("Up", &_camera.up().x, 0.5f);
        ImGui::DragFloat3("Look At", &_camera.lookAt().x, 0.5f);
        
        ImGui::SliderFloat("FOV", &_camera.fov(), 0.0f, 90.0f, "%.0f deg", 1.0f);
        ImGui::DragFloat("Aspect", &_camera.aspect(), 0.01f);
        ImGui::Text("Clipping Planes");
        {
          ImGui::Indent();
          ImGui::InputFloat("Near", &_camera.nearPlane(), 0.1f);
          ImGui::InputFloat("Far", &_camera.farPlane(), 1.0f);
          ImGui::Unindent();
        }
        ImGui::TreePop();

        if (ImGui::TreeNode("View"))
        {
          glm::mat4 view = _camera.view();
          ImGui::DragFloat4("Row 0", &view[0][0]);
          ImGui::DragFloat4("Row 1", &view[1][0]);
          ImGui::DragFloat4("Row 2", &view[2][0]);
          ImGui::DragFloat4("Row 3", &view[3][0]);
          ImGui::TreePop();
        }
      }
      ImGui::Separator();
  }
}