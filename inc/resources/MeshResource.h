#pragma once

#include "Resource.h"

namespace lava
{
  class Mesh;
  class CMeshResource : public CResource
  {
  public:
    CMeshResource();
    virtual ~CMeshResource();
    CMeshPtr GetInternal() const { return mResourceInternal; }
  protected:
    friend class CResourceManager;

    virtual bool Load(const std::string& aFilename);
    virtual void Unload();

    CMeshPtr mResourceInternal = nullptr;
  };
}
