#pragma  once

#include  "lava.h"

namespace  lava
{
  struct TransformUniformBuffer
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };
}