#pragma once

#include "lava.h"

namespace lava
{
  class Shader
  {
  public:
    Shader() = default;
    virtual ~Shader() = default;

    bool create(VkDevice _device, const void* _shaderContents, const size_t _size)
    {
      VkShaderModuleCreateInfo moduleCreateInfo{};
      moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      moduleCreateInfo.codeSize = _size;
      moduleCreateInfo.pCode = static_cast<const uint32_t*> (_shaderContents);
      return vkCreateShaderModule(_device, &moduleCreateInfo, nullptr, &mShaderModule) == VK_SUCCESS;
    }

    bool create(VkDevice _device, const char* _shaderFilename)
    {
      size_t shaderSize = 0;
      char* shaderCode = nullptr;
      std::ifstream is(_shaderFilename, std::ios::binary | std::ios::in | std::ios::ate);
      if (is.is_open())
      {
        shaderSize = is.tellg();
        is.seekg(0, std::ios::beg);
        shaderCode = new char[shaderSize];
        is.read(shaderCode, shaderSize);
        is.close();
        assert(shaderSize > 0);
      }
      bool ok = create(_device, shaderCode, shaderSize);
      delete[] shaderCode;
      return ok;
    }

    virtual void destroy(VkDevice _device)
    {
      vkDestroyShaderModule(_device, mShaderModule, nullptr);
    }

    VkShaderModule shaderModule() const { return mShaderModule; }

  private:
    VkShaderModule        mShaderModule = VK_NULL_HANDLE;
  };
}