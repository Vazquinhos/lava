#include "textures/Texture.h"
#include "render/Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "textures/stb_image.h"

namespace lava
{
  void CTexture::Create(const std::string& aFilename)
  {
    mFilename = aFilename;

    int texWidth, texHeight, texChannels;
    
    stbi_uc* pixels = stbi_load(aFilename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

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

        pixels = reinterpret_cast<stbi_uc*>(data);
      }
    }

    mImage.Create(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mImage.SetPixels(pixels);
    //mTextureId = (void *)(intptr_t)textureImage;
    stbi_image_free(pixels);
  }

  void CTexture::Destroy()
  {
    mImage.Destroy();
  }

  VkDescriptorImageInfo CTexture::GetDescriptor() const
  {
    VkDescriptorImageInfo lImageInfo = {};
    lImageInfo.imageLayout = mImage.GetLayout();
    lImageInfo.imageView = mImage.GetView();
    lImageInfo.sampler = mSampler;
    return lImageInfo;
  }
}
