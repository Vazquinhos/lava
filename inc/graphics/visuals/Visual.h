#pragma once

#include "lava.h"

#include "render/Geometry.h"
#include "render/Vertex.h"

#include "graphics/AABB.h"
#include "graphics/Transform.h"

namespace lava
{
  class Visual
  {
  public:
    Visual() = default;
    virtual ~Visual() = default;

    uint32_t geometryCount() const { return static_cast<uint32_t>(mGeometries.size()); }
    const std::shared_ptr<Geometry>& geometry(const uint32_t _idx) { return mGeometries[_idx]; }
    virtual void destroy(VkDevice _device)
    {
      for (auto & g : mGeometries)
        g->destroy(_device);
    }
    bool isVisible() const { return mVisible; }
  
  protected:
    AABB mAABB;
    Transform mTransform;
    std::vector< std::shared_ptr<Geometry> > mGeometries;
    bool mVisible;
  };
}