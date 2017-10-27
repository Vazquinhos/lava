/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  class GraphicsEntity
  {
  public:
    GraphicsEntity() = default;
    virtual ~GraphicsEntity() = default;
    std::string& name() { return mName; }
  private:
    std::string mName;
  };
}