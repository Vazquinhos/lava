#include "textures/Texture.h"
#include "render/Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "textures/stb_image.h"

namespace lava
{
  void Texture::create
  (
    VkDevice _device,
    VkPhysicalDevice _phyDevice,
    VkCommandPool _commandPool, 
    VkQueue _queue,
    VkSampler _sampler,
    const std::string& _resourceId
  )
  {
    sampler = _sampler;
    format = VK_FORMAT_R8G8B8A8_UNORM;
    tiling = VK_IMAGE_TILING_OPTIMAL;
    usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    stbi_uc* pixels = stbi_load(_resourceId.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    
    if (pixels == nullptr)
    {
      texWidth = 10;
      texHeight = 10;
      texChannels = 4;

      VkDeviceSize imageSize = texWidth * texHeight * sizeof(uint32_t);

      uint32_t *data = (uint32_t*)( std::malloc(imageSize) );
      for (int w = 0; w < texWidth; ++w)
      {
        if (w % 2 == 0)
        {
          for (int h = 0; h < texHeight; ++h)
              data[(texWidth*w) + h] = (h % 2 == 0) ? 0 : 0xFFFFFFFF;
        }
        else
        {
          for (int h = 0; h < texHeight; ++h)
            data[(texWidth*w) + h] = (h % 2 == 0) ? 0xFFFFFFFF : 0;
        }
      }

      Buffer stagingBuffer;
      stagingBuffer.create
      (
        _device,
        _phyDevice,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
      );
      stagingBuffer.update(_device, imageSize, data);

      createImage(_device, _phyDevice);

      transitionImageLayout(_device, _commandPool, _queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
      copyBufferToImage(_device, _commandPool, _queue, stagingBuffer.buffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
      transitionImageLayout(_device, _commandPool, _queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

      stagingBuffer.destroy(_device);
      createImageView(_device, VK_IMAGE_ASPECT_COLOR_BIT);

      free(data);
    }
    else
    {
      VkDeviceSize imageSize = texWidth * texHeight * 4;
      Buffer stagingBuffer;
      stagingBuffer.create
      (
        _device,
        _phyDevice,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
      );
      stagingBuffer.update(_device, imageSize, pixels);
      stbi_image_free(pixels);
      
      createImage(_device, _phyDevice);

      transitionImageLayout(_device, _commandPool, _queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
      copyBufferToImage(_device, _commandPool, _queue, stagingBuffer.buffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
      transitionImageLayout(_device, _commandPool, _queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

      stagingBuffer.destroy(_device);
      createImageView(_device, VK_IMAGE_ASPECT_COLOR_BIT);
    }
  }
}
