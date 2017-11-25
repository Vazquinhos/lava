/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  constexpr uint32_t MAX_COMPONENTS = 16;
  class Component;
  class Entity
  {
  public:
    Entity() = default;
    virtual ~Entity() = default;
  private:
    std::array<std::unique_ptr<Component>, MAX_COMPONENTS > mComponents;
  };
}