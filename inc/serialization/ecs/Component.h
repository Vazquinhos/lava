#pragma once

#include "ecs/Component.h"
#include "graphics/Camera.h"

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(lava::CCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::Component, lava::CCamera);

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const Component & t)
  {
    std::string lType = EnumString<Component::Type>::asString(t.mType);
    ar(cereal::make_nvp("type", lType));
  }

  template <class Archive>
  void load(Archive & ar, Component & t)
  {
    std::string lType;
    ar(cereal::make_nvp("type", lType));
    t.mType = EnumString<Component::Type>::asEnum(lType);
  }

  template <class Archive>
  void save(Archive & ar, const CCamera & t)
  {
    ar(cereal::make_nvp("Component", cereal::base_class<Component>(&t)));
  }

  template <class Archive>
  void load(Archive & ar, CCamera & t)
  {
    ar(cereal::make_nvp("Component", cereal::base_class<Component>(&t)));
  }
}