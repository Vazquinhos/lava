#pragma once

#include "lava.h"
#include "Debug.h"
#include "Renderer.h"

namespace lava
{
  class AABB
  {
  public:
    AABB() = default;
    virtual ~AABB() = default;

    void create(const glm::vec3& _min, const glm::vec3& _max)
    {
      mMin = _min;
      mMax = _max;
      createCorners();
    }

    AABB transformed(const glm::mat4& _model) const
    {
      AABB transformedAABB;
      glm::vec3 min = glm::vec3(std::numeric_limits<float>::infinity());
      glm::vec3 max = glm::vec3(-std::numeric_limits<float>::infinity());
      for (const glm::vec3& corner : mCorners)
      {
        glm::vec3 trsfCorner = glm::vec3(_model*glm::vec4(corner, 1));
        min.x = std::min(min.x, trsfCorner.x);
        min.y = std::min(min.y, trsfCorner.y);
        min.z = std::min(min.z, trsfCorner.z);

        max.x = std::max(max.x, trsfCorner.x);
        max.y = std::max(max.y, trsfCorner.y);
        max.z = std::max(max.z, trsfCorner.z);
      }
      transformedAABB.create(min, max);
      return transformedAABB;
    }

    const std::vector< glm::vec3 >& corners() const { return mCorners; }

  private:
    glm::vec3 mMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 mMax = glm::vec3(std::numeric_limits<float>::min());
    std::vector< glm::vec3 > mCorners;
    void createCorners()
    {
      mCorners.clear();
      mCorners.resize(8);
      mCorners[0] = mMax;
      mCorners[1] = glm::vec3(mMin.x, mMax.y, mMax.z);
      mCorners[2] = glm::vec3(mMin.x, mMin.y, mMax.z);
      mCorners[3] = glm::vec3(mMax.x, mMin.y, mMax.z);

      mCorners[4] = mMin;
      mCorners[5] = glm::vec3(mMax.x, mMin.y, mMin.z);
      mCorners[6] = glm::vec3(mMax.x, mMax.y, mMin.z);
      mCorners[7] = glm::vec3(mMin.x, mMax.y, mMin.z);
    }
  };
}