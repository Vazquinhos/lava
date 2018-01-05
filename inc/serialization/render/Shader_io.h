#pragma once

#include "ecs/Component.h"
#include "render/Shader.h"

CEREAL_REGISTER_TYPE(lava::CShader);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::CComponent, lava::CShader)

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const CShader & t)
  {
  }

  template <class Archive>
  void load(Archive & ar, CShader & t)
  {
  }
}
