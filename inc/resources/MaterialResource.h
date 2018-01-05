#pragma once

#include "Resource.h"

namespace lava
{
  class CMaterialResource : public CResource
  {
  public:
    CMaterialResource();
    virtual ~CMaterialResource();

    CMaterialPtr GetInternal() const { return mResourceInternal; }
  protected:
    friend class CResourceManager;

    virtual bool Load(const std::string& aFilename);
    virtual void Unload();

    CMaterialPtr mResourceInternal = nullptr;
  };
}
