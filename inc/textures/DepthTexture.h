#pragma once

#include "Texture.h"

namespace lava
{
  struct DepthTexture : public Texture
  {
    void create
    (
      VkExtent2D _extend,
      VkDevice _device,
      VkPhysicalDevice _phyDevice,
      VkCommandPool _commandPool,
      VkQueue _queue
    );
  };
}