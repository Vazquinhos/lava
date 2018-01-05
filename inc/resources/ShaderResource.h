#pragma once

#include "Resource.h"

namespace lava
{
  class CShaderResource : public CResource
  {
  public:
    CShaderResource();
    virtual ~CShaderResource();

    CShaderPtr GetInternal() const { return mResourceInternal; }
  protected:
    friend class CResourceManager;

    virtual bool Load(const std::string& aFilename);
    virtual void Unload();

    CShaderPtr mResourceInternal = nullptr;
  };
}
