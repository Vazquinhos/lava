#pragma once

#include "lava.h"

#include "ecs/Component.h"

namespace lava
{
  class CCamera : public CComponent
  {
    SERIALIZABLE(CCamera)
  public:
    enum class ProjectionMode
    {
      ePerspective = 0,
      eOrthografic,

      MAX
    };

    enum class ClearMode
    {
      eNothing = 0,
      eSkybox,
      eSolidColor,
      eDepth,
      
      MAX
    };

  public:
    CCamera() = default;
    virtual ~CCamera() = default;

    static CComponent::Type GetType() { return CComponent::Type::eCamera; }
    virtual std::string GetComponentId() const { return std::string("Camera"); }

    virtual void Update(float)
    {
      mView = glm::lookAt(mEye, mLookAt, mUp);
      if (mProjectionMode == ProjectionMode::eOrthografic)
      {
        mNear = std::max(-0.1f, mNear);
        mFar = std::max(0.1f, mFar);
        mProjection = glm::ortho(-1.5f * float(mViewport.b / mViewport.a), 1.5f * float(mViewport.b / mViewport.a), -1.5f, 1.5f, mNear, mFar);
      }
      else
      {
        mProjection = glm::perspective(glm::radians(mFov), mViewport.b / mViewport.a, mNear, mFar);
      }
    }

    const glm::mat4& view() const
    {
      return mView;
    }

    const glm::mat4& proj() const
    {
      return mProjection;
    }

    ProjectionMode& mode()
    {
      return mProjectionMode;
    }

    glm::vec2 worldToScreenCoordinates( const glm::vec3& _worldPoint ) const
    {
      glm::vec4 trans = mProjection * mView * glm::vec4(_worldPoint, 1);
      trans *= 0.5f / trans.w;
      trans += glm::vec4(0.5f, 0.5f,0,0);
      trans.y = 1.f - trans.y;
      trans.x *= mViewport.b;
      trans.y *= mViewport.a;
      trans.x += mViewport.r;
      trans.y += mViewport.g;
      return glm::vec2(trans.x, trans.y);
    }

    glm::vec2 worldToScreenCoordinates(const glm::vec3& _worldPoint, const glm::mat4& _modelMatrix ) const
    {
      glm::vec4 trans = mProjection * mView * _modelMatrix * glm::vec4(_worldPoint, 1);
      trans *= 0.5f / trans.w;
      trans += glm::vec4(0.5f, 0.5f, 0, 0);
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

    ProjectionMode& projectionMode() { return mProjectionMode; }
    ClearMode& clearMode() { return mClearMode; }

    const ClearMode& GetClearMode() const { return mClearMode; }
    const float3& GetClearColor() const { return mClearColor; }
    const float4& GetViewport() const { return mViewport; }

  private:
    glm::mat4 mView = float4x4(1);
    glm::mat4 mProjection = float4x4(1);
    glm::vec3 mEye = glm::vec3(40);
    glm::vec3 mLookAt = glm::vec3(0, 0, 0);
    glm::vec3 mUp = glm::vec3(0, 1, 0);
    glm::vec4 mViewport = glm::vec4(0, 0, 800.0f, 600.0f);
    glm::vec3 mClearColor = glm::vec3(0.1f, 0.1f, 0.1f);
    float mFov = 60.0f;
    float mAspect = 800.0f/600.0f;
    float mNear = 0.1f;
    float mFar = 1000.0f;

    ProjectionMode mProjectionMode = ProjectionMode::ePerspective;
    ClearMode mClearMode = ClearMode::eSolidColor;

    CCamera& operator=(const CCamera&) = delete;  // Disallow copying
    CCamera(const CCamera&) = delete;
  };

  Begin_Enum_String(CCamera::ProjectionMode)
  {
    Register_Enum_String(CCamera::ProjectionMode::ePerspective, "Perspective");
    Register_Enum_String(CCamera::ProjectionMode::eOrthografic, "Orthografic");
  }
  End_Enum_String;

  Begin_Enum_String(CCamera::ClearMode)
  {
    Register_Enum_String(CCamera::ClearMode::eNothing, "Nothing");
    Register_Enum_String(CCamera::ClearMode::eSkybox, "Skybox");
    Register_Enum_String(CCamera::ClearMode::eSolidColor, "Solid Color");
    Register_Enum_String(CCamera::ClearMode::eDepth, "Depth");
  }
  End_Enum_String;
}
