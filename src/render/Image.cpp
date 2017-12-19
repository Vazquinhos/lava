#include "render/Image.h"

#include <Logger.hpp>

#include "render/Device.h"

namespace lava
{
  namespace
  {
    bool HasStencilComponent(VkFormat aFormat)
    {
      return aFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || aFormat == VK_FORMAT_D24_UNORM_S8_UINT;
    }
  }

  void CImage::Create(uint32_t aWidth, uint32_t aHeight, VkFormat aFormat, VkImageTiling aTiling, VkImageUsageFlags aUsage, VkImageAspectFlags aAspectFlags, VkMemoryPropertyFlags aProperties)
  {
    mWidth = aWidth;
    mHeight = aHeight;
    mFormat = aFormat;
    mTiling = aTiling;
    mUsageFlags = aUsage;
    mAspectFlags = aAspectFlags;

    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    // Create image and bind with its memory

    vkNew(VkImageCreateInfo, VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, lImageInfo);

    lImageInfo.imageType = VK_IMAGE_TYPE_2D;
    lImageInfo.extent.width = aWidth;
    lImageInfo.extent.height = aHeight;
    lImageInfo.extent.depth = 1;
    lImageInfo.mipLevels = 1;
    lImageInfo.arrayLayers = 1;
    lImageInfo.format = aFormat;
    lImageInfo.tiling = aTiling;
    lImageInfo.initialLayout = mImageLayout;
    lImageInfo.usage = aUsage;
    lImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    lImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCall(vkCreateImage(lLogicalDevice, &lImageInfo, nullptr, &mImage));

    VkMemoryRequirements llMemoryRequirements;
    vkGetImageMemoryRequirements(lLogicalDevice, mImage, &llMemoryRequirements);

    vkNew(VkMemoryAllocateInfo, VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, lAllocInfo);
    lAllocInfo.allocationSize = llMemoryRequirements.size;
    lAllocInfo.memoryTypeIndex = lDevice.FindMemoryType(llMemoryRequirements.memoryTypeBits, aProperties);

    vkCall(vkAllocateMemory(lLogicalDevice, &lAllocInfo, nullptr, &mImageMemory));

    vkBindImageMemory(lLogicalDevice, mImage, mImageMemory, 0);

    // Create Image view

    vkNew(VkImageViewCreateInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, lViewInfo);

    lViewInfo.image = mImage;
    lViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    lViewInfo.format = aFormat;
    lViewInfo.subresourceRange.aspectMask = mAspectFlags;
    lViewInfo.subresourceRange.baseMipLevel = 0;
    lViewInfo.subresourceRange.levelCount = 1;
    lViewInfo.subresourceRange.baseArrayLayer = 0;
    lViewInfo.subresourceRange.layerCount = 1;

    vkCall(vkCreateImageView(lLogicalDevice, &lViewInfo, nullptr, &mImageView));
  }

  void CImage::Transition(VkImageLayout aNewLayout)
  {
    CDevice& lDevice = CDevice::getInstance();

    VkCommandBuffer lCommandBuffer = lDevice.BeginSingleExecutionCommand();

    vkNew(VkImageMemoryBarrier, VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, lBarrier);

    lBarrier.oldLayout = mImageLayout;
    lBarrier.newLayout = aNewLayout;
    lBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    lBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    lBarrier.image = mImage;

    if (aNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      lBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

      if (HasStencilComponent(mFormat))
        lBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
      lBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    lBarrier.subresourceRange.baseMipLevel = 0;
    lBarrier.subresourceRange.levelCount = 1;
    lBarrier.subresourceRange.baseArrayLayer = 0;
    lBarrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (mImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && aNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
      lBarrier.srcAccessMask = 0;
      lBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (mImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && aNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
      lBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      lBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (mImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && aNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
      lBarrier.srcAccessMask = 0;
      lBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
      lavaAssert(false,"unsupported layout transition!");
    }

    mImageLayout = aNewLayout;

    vkCmdPipelineBarrier(
      lCommandBuffer,
      sourceStage, destinationStage,
      0,
      0, nullptr,
      0, nullptr,
      1, &lBarrier
    );

    lDevice.EndSingleExecutionCommand(lCommandBuffer);
  }

  void CImage::Destroy()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    vkDestroyImageView(lLogicalDevice, mImageView, nullptr);
    vkDestroyImage(lLogicalDevice, mImage, nullptr);
    vkFreeMemory(lLogicalDevice, mImageMemory, nullptr);
  }

  void CImage::SetPixels(uint8_t* aPixels)
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    // Modifiy the layout of the image
    Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Create a stagin buffer to copy the data of the pixels to the image in the GPU
    vkNew(VkBufferCreateInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, lBufferInfo)
    
    lavaAssert(VK_FORMAT_R8G8B8A8_UNORM == mFormat, "The pixels size is incorrect");

    lBufferInfo.size = mWidth * mHeight * 4; // This is because the format is VK_FORMAT_R8G8B8A8_UNORM

    lBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    lBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Create a host-visible staging buffer that contains the raw image data
    VkBuffer lStagingBuffer;
    VkDeviceMemory lStagingMemory;

    vkCall(vkCreateBuffer(lLogicalDevice, &lBufferInfo, nullptr, &lStagingBuffer));

    // Get memory requirements for the staging buffer (alignment, memory type bits)
    VkMemoryRequirements lMemoryRequirements;
    vkGetBufferMemoryRequirements(lLogicalDevice, lStagingBuffer, &lMemoryRequirements);

    vkNew(VkMemoryAllocateInfo, VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, lAllocInfo);
    lAllocInfo.allocationSize = lMemoryRequirements.size;
    // Get memory type index for a host visible buffer
    lAllocInfo.memoryTypeIndex = lDevice.FindMemoryType(lMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkCall(vkAllocateMemory(lLogicalDevice, &lAllocInfo, nullptr, &lStagingMemory));
    vkCall(vkBindBufferMemory(lLogicalDevice, lStagingBuffer, lStagingMemory, 0));

    // Copy texture data into staging buffer
    uint8_t *data;
    vkCall(vkMapMemory(lLogicalDevice, lStagingMemory, 0, lMemoryRequirements.size, 0, (void **)&data));
    memcpy(data, aPixels, lBufferInfo.size);
    vkUnmapMemory(lLogicalDevice, lStagingMemory);

    VkCommandBuffer commandBuffer = lDevice.BeginSingleExecutionCommand();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { mWidth,mHeight, 1 };

    vkCmdCopyBufferToImage(commandBuffer, lStagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    lDevice.EndSingleExecutionCommand(commandBuffer);

    vkDestroyBuffer(lLogicalDevice, lStagingBuffer, nullptr);
    vkFreeMemory(lLogicalDevice, lStagingMemory, nullptr);

    Transition(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }
}
