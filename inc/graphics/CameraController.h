#pragma once

#include "lava.h"

namespace lava
{
  class CCamera;
  class CCameraController
  {
  public:
    enum class Mode
    {
      eFree = 0,
      eSpherical,
    };

  public:
    CCameraController() = default;
    virtual ~CCameraController() = default;

    void setControllCCamera(CCamera* _CCamera);
    void update(float _dt);

    float& xSpeed() { return mXSpeed; }
    float& ySpeed() { return mYSpeed; }
    float& shiftSpeed() { return mShiftSpeed; }
    float& speed() { return mSpeed; }

  private:
    CCamera* mCCamera;
    float mShiftSpeed = 0.1f;
    float mSpeed = 0.01f;
    float mXSpeed = -0.001f;
    float mYSpeed = -0.001f;
  };
}