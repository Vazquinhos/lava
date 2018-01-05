/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "ecs/Component.h"

namespace lava
{
  class Transform : public CComponent
  {
    SERIALIZABLE(Transform)
  public:
    Transform()
      : CComponent()
    {
      decompose();
    }

    static CComponent::Type GetType() { return CComponent::Type::eTransform; }
    virtual std::string GetComponentId() const { return std::string("Transform"); }

    virtual ~Transform() = default;
    
    void decompose()
    {
      glm::decompose(mTransformation, mScale, mRotation, mPosition, mSkew, mPerspective);
    }

    void recompose()
    {
      mTransformation = glm::mat4_cast(mRotation);

      mTransformation[3][0] = mPosition.x;
      mTransformation[3][1] = mPosition.y;
      mTransformation[3][2] = mPosition.z;

      mTransformation = glm::scale(mTransformation, mScale);
    }

    glm::vec3 & position() { return mPosition; }
    glm::quat & rotation() { return mRotation; }
    glm::vec3 & scale() { return mScale; }

    float* mat()
    {
      return &mTransformation[0][0];
    }

    const glm::mat4& transformation() const
    {
      return mTransformation;
    }

    glm::vec3 apply(const glm::vec3& _other)
    {
      glm::vec4 auxVec4 = mTransformation * glm::vec4(_other, 1);
      return glm::vec3(auxVec4);
    }

  private:
    glm::vec3 mScale;
    glm::quat mRotation;
    glm::vec3 mSkew;
    glm::vec4 mPerspective;
    glm::vec3 mPosition;
    glm::mat4 mTransformation = glm::mat4(1);
  };
}