#pragma once

#include "lava.h"

#include "render/Image.h"

namespace lava
{
  class CTextureResource;
  class CTexture
  {
  public:
    CTexture() = default;
    virtual ~CTexture() = default;

    void SetSampler(VkSampler aSampler) { mSampler = aSampler; }

    VkDescriptorImageInfo GetDescriptor() const;

    const std::string& GetId() const { return mFilename; }

  private:

    friend class CTextureResource;

    std::string mFilename;
    void* mTextureId = nullptr;

    CImage mImage;
    VkSampler mSampler;
  };
}