/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"
#include "Entity.h"

#include <Singleton.hpp>
#include <serialization/serialization.h>

namespace lava
{
  class World : public Singleton<World>
  {
    SERIALIZABLE(World)

  public:
    World() = default;
    virtual ~World() = default;

    void update(float _dt);
    void render();

    Entity* getNewEntity( const std::string& _name )
    {
      mEntities.emplace_back(std::make_unique<Entity>());
      Entity* entity = mEntities[mEntities.size() - 1].get();
      entity->name() = _name;
      mEntitiesMap[_name] = mEntities.size() - 1;
      return entity;
    }

    const size_t length() const
    {
      return mEntities.size();
    }

    Entity* find(const std::string& _name) const
    {
      std::map< std::string, size_t >::const_iterator it = mEntitiesMap.find(_name);
      return (it == mEntitiesMap.end() ) ? nullptr : at(it->second);
    }

    Entity* at(const size_t idx) const
    {
      return mEntities[idx].get();
    }

  private:
    std::map<std::string, size_t> mEntitiesMap;
    std::vector< std::unique_ptr<Entity> > mEntities;
  };
}