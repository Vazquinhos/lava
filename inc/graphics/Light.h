#pragma once

#include "lava.h"

namespace lava
{
  class Light
  {
  public:
    Light() = default;
    virtual ~Light() = default;

    void create
    (
      const glm::vec3& _position,
      const glm::vec3& _color,
      const glm::vec2& _rangeAttenuation,
      const float _intensity
    )
    {
       
    }

  private:
  };
}