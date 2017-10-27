/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  template<class T, uint32_t maxCountPool>
  class ObjectPool
  {
  public:
    ObjectPool()
    {
      for (auto& object : mObjects)
        object = std::make_shared<T>();
    }

    virtual ~ObjectPool() = default;

    std::shared_ptr<T> newObject()
    {
      size_t o = 0;
      size_t count = 0;
      std::shared_ptr<T> object;
      do
      {
        object = mObjects[firstAvailable++];
        if (firstAvailable == maxCountPool)
          firstAvailable = 0;
      } while (object.use_count() > 2 && o++ < maxCountPool);

      if (object.use_count() != 2)
      {
        object = std::make_shared<T>();
        warningLog(std::string(std::string(typeid(this).name()) + std::string("->This allocation exceeds the maximum number of allocations." )));
      }
      
      return object;
    }

    private:
      size_t firstAvailable = 0;
      std::array< std::shared_ptr<T> , maxCountPool > mObjects;
  };

  template<class T> std::shared_ptr<T> NewObject();

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

    std::unordered_map<std::string, size_t > mCamerasMap;
    std::vector< std::pair< bool, Texture > > mCameras;

  };
}