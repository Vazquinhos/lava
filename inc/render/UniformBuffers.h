#pragma  once

#include  "lava.h"

#include "render/Buffer.h"

namespace  lava
{
  struct TransformUBO
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  struct LightUBO
  {
    glm::vec3 position;
    float beginRange;
    glm::vec3 direction;
    float endRange;
    glm::vec3 color;
    float intensity;
  };
}