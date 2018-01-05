#pragma once

#include "Resource.h"

namespace lava
{
  class CTextureResource : public CResource
  {
  public:
    CTextureResource();
    virtual ~CTextureResource();

    CTexturePtr GetInternal() const { return mResourceInternal; }
  protected:
    friend class CResourceManager;

    virtual bool Load(const std::string& aFilename);
    virtual void Unload();

    CTexturePtr mResourceInternal = nullptr;
  };
}
