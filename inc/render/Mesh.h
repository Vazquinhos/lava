#pragma once

#include "render/Visual.h"

namespace lava
{
  class CMeshResource;
  class CMesh : public CVisual
  {
   SERIALIZABLE(CMesh)
  public:
    CMesh() = default;
    virtual ~CMesh() = default;
  protected:

    friend class CMeshResource;
  };
}