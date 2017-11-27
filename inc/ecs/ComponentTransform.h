/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "Component.h"

namespace lava
{
  class Transform : public Component
  {
  public:
    Transform()
      : Component(Type::eTransform)
    {
      glm::decompose(mTransformation, mScale, mRotation, mPosition, mSkew, mPerspective);
    }

    virtual ~Transform() = default;

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
  private:
    glm::vec3 mScale;
    glm::quat mRotation;
    glm::vec3 mSkew;
    glm::vec4 mPerspective;
    glm::vec3 mPosition;
    glm::mat4 mTransformation = glm::mat4(1);
  };
}