/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  constexpr uint32_t MAX_ENTITIES = 1024;
  class Entity;
  class World
  {
  public:
    World() = default;
    virtual ~World() = default;

  private:
    std::array< Entity, MAX_ENTITIES > mEntities;
  };
}