#pragma once

#include "lava.h"

namespace lava
{
  class Light
  {
  public:
    Light() = default;
    virtual ~Light() = default;

    glm::vec3& position() { return mPosition; }
    glm::vec3& color() { return mColor; }
    glm::vec2& rangeAttenuation () { return mRangeAttenuation; }
    float&     intensity() { return mIntensity; }
    
  private:
    glm::vec3 mPosition;
    glm::vec3 mColor;
    glm::vec2 mRangeAttenuation;
    float mIntensity;
  };
}