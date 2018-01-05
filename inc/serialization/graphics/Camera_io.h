#pragma once

#include "ecs/Component.h"
#include "graphics/Camera.h"

CEREAL_REGISTER_TYPE(lava::CCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::CComponent, lava::CCamera)

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const CCamera & t)
  {
    //ar(cereal::make_nvp("Component", cereal::base_class<Component>(&t)));
    std::string lType = EnumString<CCamera::ProjectionMode>::asString(t.mProjectionMode);
    ar(cereal::make_nvp("projection", lType));
    ar(cereal::make_nvp("viewport", t.mViewport));

    lType = EnumString<CCamera::ClearMode>::asString(t.mClearMode);
    ar(cereal::make_nvp("clear_mode", lType));

    ar(cereal::make_nvp("clear_color", t.mClearColor));
    ar(cereal::make_nvp("fov", t.mFov));
    ar(cereal::make_nvp("near", t.mNear));
    ar(cereal::make_nvp("far", t.mFar));
  }

  template <class Archive>
  void load(Archive & ar, CCamera & t)
  {
    //ar(cereal::make_nvp("Component", cereal::base_class<Component>(&t)));
    std::string lType;
    ar(cereal::make_nvp("projection", lType));
    t.mProjectionMode = EnumString<CCamera::ProjectionMode>::asEnum(lType);

    ar(cereal::make_nvp("projection", lType));
    ar(cereal::make_nvp("viewport", t.mViewport));

    ar(cereal::make_nvp("clear_mode", lType));
    t.mClearMode = EnumString<CCamera::ClearMode>::asEnum(lType);

    ar(cereal::make_nvp("clear_color", t.mClearColor));
    ar(cereal::make_nvp("fov", t.mFov));
    ar(cereal::make_nvp("near", t.mNear));
    ar(cereal::make_nvp("far", t.mFar));
  }
}
