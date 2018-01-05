#pragma once

#include "ecs/Entity.h"

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const CEntity & t)
  {
    ar(cereal::make_nvp("name", t.mName));
    ar(cereal::make_nvp("components", t.mComponents));
  }

  template <class Archive>
  void load(Archive & ar, CEntity & t)
  {
    ar(cereal::make_nvp("name", t.mName));
    ar(cereal::make_nvp("components", t.mComponents));
    t.Refresh();
  }
}
