/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"
#include "Entity.h"

namespace lava
{
  class World
  {
  public:
    World() = default;
    virtual ~World() = default;

    void update(float _dt);
    void render();

    Entity* getNewEntity()
    {
      mEntities.emplace_back(std::make_unique<Entity>());
      return mEntities[mEntities.size() - 1].get();
    }

    const size_t length()
    {
      return mEntities.size();
    }

    Entity* at(const size_t idx)
    {
      return mEntities[idx].get();
    }

  private:
    std::vector< std::unique_ptr<Entity> > mEntities;
  };
}