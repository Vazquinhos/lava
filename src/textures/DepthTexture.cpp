#include "textures/DepthTexture.h"

namespace lava
{
  namespace
  {
    bool IsCorrectDepthFormat(VkPhysicalDevice _phyDevice, VkImageTiling _tiling, VkFormat _dephtFormat )
    {
      VkFormatProperties props;
      vkGetPhysicalDeviceFormatProperties(_phyDevice, _dephtFormat, &props);

      if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        return true;
      }
      else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        return true;
      }

      return false;
    }
  }

  void DepthTexture::create
  (
    VkExtent2D _extend,
    VkDevice _device,
    VkPhysicalDevice _phyDevice,
    VkCommandPool _commandPool,
    VkQueue _queue
  )
  {
    tiling = VK_IMAGE_TILING_OPTIMAL;
    usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    texWidth = _extend.width;

    if (IsCorrectDepthFormat(_phyDevice, tiling, VK_FORMAT_D32_SFLOAT))
      format = VK_FORMAT_D32_SFLOAT;
    else if (IsCorrectDepthFormat(_phyDevice, tiling, VK_FORMAT_D32_SFLOAT_S8_UINT))
      format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    else if (IsCorrectDepthFormat(_phyDevice, tiling, VK_FORMAT_D24_UNORM_S8_UINT))
      format = VK_FORMAT_D24_UNORM_S8_UINT;

    createImage(_device, _phyDevice);
    createImageView(_device, VK_IMAGE_ASPECT_DEPTH_BIT);

    transitionImageLayout(_device, _commandPool, _queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }
}
