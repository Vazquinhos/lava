#pragma once

#include "ecs/Component.h"
#include "graphics/Light.h"

CEREAL_REGISTER_TYPE(lava::Light);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::CComponent, lava::Light)

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const Light & t)
  {
  }

  template <class Archive>
  void load(Archive & ar, Light & t)
  {
  }
}
