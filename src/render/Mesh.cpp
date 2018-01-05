#pragma once

#include "lava.h"

#include "render/Geometry.h"
#include "render/Vertex.h"

#include "graphics/AABB.h"
#include "ecs/Component.h"


namespace lava
{
  class Visual : public CComponent
  {
   
  public:
    Visual() = default;
    virtual ~Visual() = default;

    static CComponent::Type GetType() { return CComponent::Type::eVisual; }
    virtual std::string GetComponentId() const { return std::string("Visual"); }

    uint32_t geometryCount() const { return static_cast<uint32_t>(mGeometries.size()); }
    const std::shared_ptr<Geometry>& geometry(const uint32_t _idx) { return mGeometries[_idx]; }
    virtual void destroy(VkDevice _device)
    {
      for (auto & g : mGeometries)
        g->destroy(_device);
    }
    const AABB& aabb() const { return mAABB; }
  
  protected:
    AABB mAABB;
    std::vector< std::shared_ptr<Geometry> > mGeometries;
  };
}