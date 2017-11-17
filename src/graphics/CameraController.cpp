#include "graphics/CameraController.h"
#include "graphics/Camera.h"

#include "imgui/imgui_impl.h"

namespace lava
{
  void CameraController::setControllCamera(Camera*_camera)
  {
    mCamera = _camera;
  }

  void CameraController::update(float _dt)
  {
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseDown[1])
    {
      // Obtain the basis of the camera
      glm::vec3 forward = glm::normalize(mCamera->lookAt() - mCamera->eye());
      glm::vec3 up(0.0, 0.0, 1.0);
      glm::vec3 right = glm::cross(forward, up);
      // Make it orthogonal
      up = glm::cross(right, forward);

      // Movement of the camera
      float velocity = (io.KeyShift) ? mShiftSpeed : mSpeed;
      float forwardAmount = (io.KeysDown[87]) ? 1.0f : 0.0f + (io.KeysDown[83]) ? -1.0f : 0.0f;
      float horizontalAmount = (io.KeysDown[68]) ? 1.0f : 0.0f + (io.KeysDown[65]) ? -1.0f : 0.0f;
      float verticalAmount = (io.KeysDown[81]) ? 1.0f : 0.0f + (io.KeysDown[69]) ? -1.0f : 0.0f;
      if (glm::abs(forwardAmount + horizontalAmount + verticalAmount) > 0.0f)
      {
        glm::vec3 forwardAxis = forward * forwardAmount;
        glm::vec3 rightAxis = right * horizontalAmount;
        glm::vec3 upAxis = up * verticalAmount;
        glm::vec3 displacement = glm::normalize(forwardAxis + rightAxis + upAxis);

        mCamera->eye(mCamera->eye() + velocity * displacement);
      }

      // Rotation of the camera
      glm::quat yawQuat = glm::angleAxis(io.MouseDelta.x * mXSpeed, up);
      glm::quat pitchQuat = glm::angleAxis(io.MouseDelta.y * mYSpeed, right);

      forward = glm::cross(pitchQuat, yawQuat)*forward;

      mCamera->lookAt(mCamera->eye() + forward);
    }
  }
}