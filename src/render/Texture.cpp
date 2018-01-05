#include "render/Texture.h"

#include "render/Device.h"

namespace lava
{
  VkDescriptorImageInfo CTexture::GetDescriptor() const
  {
    VkDescriptorImageInfo lImageInfo = {};
    lImageInfo.imageLayout = mImage.GetLayout();
    lImageInfo.imageView = mImage.GetView();
    if (mSampler == nullptr)
    {
      VkSamplerCreateInfo samplerInfo = {};
      samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerInfo.magFilter = VK_FILTER_LINEAR;
      samplerInfo.minFilter = VK_FILTER_LINEAR;
      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.anisotropyEnable = VK_TRUE;
      samplerInfo.maxAnisotropy = 16;
      samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
      samplerInfo.unnormalizedCoordinates = VK_FALSE;
      samplerInfo.compareEnable = VK_FALSE;
      samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

      vkCall(vkCreateSampler(CDevice::getInstance().GetLogicalDevice(), &samplerInfo, nullptr, &lImageInfo.sampler));
    }
    else
      lImageInfo.sampler = mSampler;
    return lImageInfo;
  }
}
