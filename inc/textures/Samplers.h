#pragma once

#include "lava.h"

namespace lava
{
  namespace Samplers
  {
    enum class Type
    {
      eLinearRepeat = 0,
      ePointRepeat,

      MAX
    };

    static VkSampler sLinearSampler;
    static VkSampler sPointSampler;

    static void create()
    {
      CDevice& lDevice = CDevice::getInstance();
      VkDevice lLogicalDevice = lDevice.GetLogicalDevice();
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

        vkCall(vkCreateSampler(lLogicalDevice, &samplerInfo, nullptr, &sLinearSampler));
      }
      
      {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
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

        vkCall(vkCreateSampler(lLogicalDevice, &samplerInfo, nullptr, &sPointSampler));
      }
    }

    static void destroy()
    {
      CDevice& lDevice = CDevice::getInstance();  
      VkDevice lLogicalDevice = lDevice.GetLogicalDevice();
      vkDestroySampler(lLogicalDevice, sPointSampler, nullptr);
      vkDestroySampler(lLogicalDevice, sLinearSampler, nullptr);
    }
  };
}