#pragma once

#include "ecs/Component.h"
#include "graphics/Transform.h"

CEREAL_REGISTER_TYPE(lava::Transform);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::CComponent, lava::Transform)

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const CComponent & t)
  {
    std::string lType = EnumString<CComponent::Type>::asString(t.mType);
    ar(cereal::make_nvp("type", lType));
  }

  template <class Archive>
  void load(Archive & ar, CComponent & t)
  {
    std::string lType;
    ar(cereal::make_nvp("type", lType));
    t.mType = EnumString<CComponent::Type>::asEnum(lType);
  }

  template <class Archive>
  void save(Archive & ar, const Transform & t)
  {
    ar(cereal::make_nvp("translation", t.mPosition));
    ar(cereal::make_nvp("rotation", t.mRotation));
    ar(cereal::make_nvp("scale", t.mScale));
  }

  template <class Archive>
  void load(Archive & ar, Transform & t)
  {
    ar(cereal::make_nvp("translation", t.mPosition));
    ar(cereal::make_nvp("rotation", t.mRotation));
    ar(cereal::make_nvp("scale", t.mScale));
  }
}
