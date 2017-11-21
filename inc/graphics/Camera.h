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

    void updateMatrices()
    {
      mView = glm::lookAt(mEye, mLookAt, mUp);
      if (mMode == eOrthografic)
      {
        mProjection = glm::ortho(-1.5f * float(mViewport.b / mViewport.a), 1.5f * float(mViewport.b / mViewport.a), -1.5f, 1.5f, -10.0f, 10.f);
      }
      else
      {
        mProjection = glm::perspective(glm::radians(mFov), mViewport.b/mViewport.a, mNear, mFar);
      }

      //mProjection[1][1] *= -1;
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
          mFar = 1000.0f;
        }
        else
        {
          mNear = -10.0;
          mFar = 10.0f;
        }
      }
      return *this;
    }

    glm::vec2 worldToScreenCoordinates( const glm::vec3& _worldPoint )
    {
      glm::mat4 projT = glm::transpose(mProjection);
      glm::mat4 viewT = glm::transpose(mView);
      glm::vec4 trans = glm::mat4(1) * mView * mProjection * glm::vec4(_worldPoint, 1);
      trans *= 0.5f / trans.w;
      trans += glm::vec4(0.5f, 0.5f,0,0);
      trans.y = 1.f - trans.y;
      trans.x *= mViewport.b;
      trans.y *= mViewport.a;
      trans.x += mViewport.r;
      trans.y += mViewport.g;
      return glm::vec2(trans.x, trans.y);
    }

    glm::vec3& eye() { return mEye; }
    glm::vec3& lookAt() { return mLookAt; }
    glm::vec3& up() { return mUp; }
    glm::vec4& viewport() { return mViewport; }

    float& fov() { return mFov; }
    float& nearPlane() { return mNear; }
    float& farPlane() { return mFar; }

  private:
    glm::mat4 mView;
    glm::mat4 mProjection;
    glm::vec3 mEye;
    glm::vec3 mLookAt;
    glm::vec3 mUp = glm::vec3(0, 0, 1);
    glm::vec4 mViewport = glm::vec4(0, 0, 800.0f, 600.0f);
    float mFov = 60.0f;
    float mAspect = 800.0f/600.0f;
    float mNear = 0.1f;
    float mFar = 1000.0f;

    CameraMode mMode = ePerspective;
  };
}