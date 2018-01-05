#include "render/Shader.h"

#include "render/Device.h"

namespace lava
{
    void CShader::Create(const void* aShaderContents, const size_t aSize)
    {
      lavaAssert(aShaderContents != nullptr && aSize > 0, "Invalid shader code");
      CDevice& lDevice = lava::CDevice::getInstance();
      VkDevice lLogicalDevice = lDevice.GetLogicalDevice();
      VkShaderModuleCreateInfo moduleCreateInfo{};
      moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      moduleCreateInfo.codeSize = aSize;
      moduleCreateInfo.pCode = static_cast<const uint32_t*> (aShaderContents);
      vkCall(vkCreateShaderModule(lLogicalDevice, &moduleCreateInfo, nullptr, &mShaderModule));
    }

    void CShader::Destroy()
    {
      CDevice& lDevice = lava::CDevice::getInstance();
      VkDevice lLogicalDevice = lDevice.GetLogicalDevice();
      vkDestroyShaderModule(lLogicalDevice, mShaderModule, nullptr);
    }
}