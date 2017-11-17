#pragma once

#include "lava.h"

namespace lava
{
  class Camera;
  class CameraController
  {
  public:
    enum class Mode
    {
      eFree = 0,
      eSpherical,
    };

  public:
    CameraController() = default;
    virtual ~CameraController() = default;

    void setControllCamera(Camera* _camera);
    void update(float _dt);

    float& xSpeed() { return mXSpeed; }
    float& ySpeed() { return mYSpeed; }
    float& shiftSpeed() { return mShiftSpeed; }
    float& speed() { return mSpeed; }

  private:
    Camera* mCamera;
    float mShiftSpeed = 0.1f;
    float mSpeed = 0.01f;
    float mXSpeed = -0.001f;
    float mYSpeed = -0.001f;
  };
}