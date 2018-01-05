/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

#include <Singleton.hpp>
#include <ObjectFactory.hpp>

namespace lava
{
  class Component;
  class CComponentFactory : public Singleton<CComponentFactory>, public ObjectFactory< CComponent >
  {
  public:
    CComponentFactory();
    virtual ~CComponentFactory() = default;
  };
}