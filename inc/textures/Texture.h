#pragma once

#include "lava.h"

#include "render/Image.h"

namespace lava
{
  class CTexture
  {
  public:
    CTexture() = default;
    virtual ~CTexture() = default;

    void Create(const std::string& aFilename);
    void Destroy();

    void SetSampler(VkSampler aSampler) { mSampler = aSampler; }

    VkDescriptorImageInfo GetDescriptor() const;

  private:

    std::string mFilename;
    void* mTextureId = nullptr;

    CImage mImage;
    VkSampler mSampler;
  };
}