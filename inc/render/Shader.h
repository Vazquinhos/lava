#pragma once

#include "lava.h"

namespace lava
{
  class CShader
  {
  public:
    CShader() = default;
    virtual ~CShader() = default;

    void Create(const void* _shaderContents, const size_t _size);
    void Destroy();

    const std::string& GetId() const { return mFilename; }
    VkShaderModule GetShaderModule() const { return mShaderModule; }

  private:
    VkShaderModule        mShaderModule = VK_NULL_HANDLE;

    std::string mFilename;
  };
}