#pragma once

#include "ecs/World.h"

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const CWorld & t)
  {
    ar(cereal::make_nvp("entities", t.mEntities));
  }

  template <class Archive>
  void load(Archive & ar, CWorld & t)
  {
    ar(cereal::make_nvp("entities", t.mEntities));
    t.Refresh();
  }
}
