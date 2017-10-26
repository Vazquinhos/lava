/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  class Texture;
  class GraphicsManager
  {
  public:
    GraphicsManager() = default;
    virtual ~GraphicsManager() = default;

    Texture* GetTexture( const std::string& _textureName ) const
    {
      Texture* texture = nullptr;
      auto& it = mTexturesMap.find(_textureName);
      if (it == mTexturesMap.end())
      {

      }
      else
      {
        
      }
      return texture;
    }

  private:

    std::unordered_map<std::string, size_t > mTexturesMap;
    std::vector< std::pair< bool, Texture > > mTextures;

  };
}