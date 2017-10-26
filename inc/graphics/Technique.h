/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "Shader.h"

namespace lava
{
  class Technique
  {
  public:
    Technique() = default;
    virtual ~Technique() = default;

    Technique& shader(const VkShaderStageFlagBits& _stage, Shader* _shader)
    {
      mShaders.push_back(std::pair<VkShaderStageFlagBits, Shader*>(_stage, _shader));
      return *this;
    }

    const uint32_t shaderCount() const
    {
      return static_cast<uint32_t>(mShaders.size());
    }

    const std::pair< VkShaderStageFlagBits, Shader* >& shaderAt(const uint32_t _idx) const
    {
      return mShaders[static_cast<uint32_t>(_idx)];
    }

  private:
    std::vector< std::pair< VkShaderStageFlagBits, Shader* > > mShaders;
  };
}