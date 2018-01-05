#pragma once

#include "lava.h"

#pragma warning( push )
#pragma warning( disable :4996)

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/base_class.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/polymorphic.hpp>

#pragma warning( pop ) 

namespace glm
{
  template<typename Archive> void serialize(Archive& archive, glm::vec3& v3)
  {
    archive(cereal::make_nvp("x", v3.x), cereal::make_nvp("y", v3.y), cereal::make_nvp("z", v3.z));
  }

  template<typename Archive> void serialize(Archive& archive, glm::vec4& v4)
  {
    archive(cereal::make_nvp("x", v4.x), cereal::make_nvp("y", v4.y), cereal::make_nvp("z", v4.z), cereal::make_nvp("w", v4.w));
  }

  template<typename Archive> void serialize(Archive& archive, glm::quat& q)
  {
    archive(cereal::make_nvp("x", q.x), cereal::make_nvp("y", q.y), cereal::make_nvp("z", q.z), cereal::make_nvp("w", q.w));
  }
}