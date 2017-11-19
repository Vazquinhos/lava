#pragma once

#include "lava.h"

#include "GraphicsEntity.h"

namespace lava
{
  class Camera : public GraphicsEntity
  {
  public:
    Camera() = default;
    virtual ~Camera() = default;

    void create
    (
      const glm::vec3& _eye,
      const glm::vec3& _lookAt,
      const glm::vec3& _up = glm::vec3(0.0, 0.0, 1.0),
      float _fov = 60.0,
      float _aspect = 800.0 / 600,
      float _near = 0.1f,
      float _far = 100000.0f
    )
    {
      mEye    = _eye;
      mLookAt = _lookAt;
      mUp     = _up;
      mFov    = _fov;
      mAspect = _aspect;
      mFar = _far;
      mNear = _near;
    }

    void updateMatrices()
    {
      mView = glm::lookAt(mEye, mLookAt, mUp);
      if (mMode == eOrthografic)
      {
        mProjection = glm::ortho(-1.5f * float(mAspect), 1.5f * float(mAspect), -1.5f, 1.5f, -10.0f, 10.f);
      }
      else
      {
        mProjection = glm::perspective(glm::radians(mFov), mAspect, mNear, mFar);
      }

      mProjection[1][1] *= -1;
    }

    const glm::mat4& view() const
    {
      return mView;
    }

    const glm::mat4& proj() const
    {
      return mProjection;
    }

    Camera& mode(const CameraMode& _mode)
    {
      if (mMode != _mode)
      {
        mMode = _mode;
        if (mMode == ePerspective)
        {
          mNear = 0.001f;
          mFar = 10000.0f;
        }
        else
        {
          mNear = -10.0;
          mFar = 10.0f;
        }
      }
      return *this;
    }

    glm::vec3& eye() { return mEye; }
    glm::vec3& lookAt() { return mLookAt; }
    glm::vec3& up() { return mUp; }

    float& fov() { return mFov; }
    float& aspect() { return mAspect; }
    float& nearPlane() { return mNear; }
    float& farPlane() { return mFar; }

  private:
    glm::mat4 mView;
    glm::mat4 mProjection;
    glm::vec3 mEye;
    glm::vec3 mLookAt;
    glm::vec3 mUp;
    float mFov;
    float mAspect;
    float mNear;
    float mFar;

    CameraMode mMode = ePerspective;
  };
}