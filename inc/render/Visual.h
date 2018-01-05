#pragma once

#include "lava.h"

#include "render/Geometry.h"
#include "render/Vertex.h"

#include "graphics/AABB.h"

namespace lava
{
  class CVisual
  {

  public:
    CVisual() = default;
    virtual ~CVisual() = default;

    size_t GetGeometryCount() const { return static_cast<uint32_t>(mGeometries.size()); }
    std::shared_ptr< Geometry > GetGeometryByIdx(const size_t aIdx) const { return mGeometries[aIdx]; }
    const AABB& GetAABB() const { return mAABB; }

  protected:
    AABB mAABB;
    std::vector< std::shared_ptr< Geometry > > mGeometries;
  };
}