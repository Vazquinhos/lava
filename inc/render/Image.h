#pragma once

#include "lava.h"

namespace lava
{
  class CImage
  {
  public:
    CImage() = default;
    virtual ~CImage() = default;

    void Create(uint32_t aWidth, uint32_t aHeight, VkFormat aFormat, VkImageTiling aTiling, VkImageUsageFlags aUsage, VkImageAspectFlags aAspectFlags, VkMemoryPropertyFlags aProperties);
    void Transition(VkImageLayout aNewLayout);
    void SetPixels(uint8_t* aPixels);
    void Destroy();

    VkImageView GetView() const { return mImageView; }
    VkImageLayout GetLayout() const { return mImageLayout; }

  private:
    uint32_t mWidth;
    uint32_t mHeight;
    VkFormat mFormat;
    VkImageTiling mTiling;
    VkImageUsageFlags mUsageFlags;
    VkImageAspectFlags mAspectFlags;
    VkImage mImage;
    VkDeviceMemory mImageMemory;
    VkImageView mImageView;
    VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  };
}
