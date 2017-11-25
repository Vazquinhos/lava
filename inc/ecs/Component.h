/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

#include <EnumStringConversor.hpp>

namespace lava
{
  class Component
  {
  public:
    enum class Type
    {
      eTransform = 0,
      eVisual,
      eMaterial,

      MAX
    };
  public:
    Component() = default;
    virtual ~Component() = default;
  private:
  };

  Begin_Enum_String(Component::Type)
  {
    Register_Enum_String(Component::Type::eTransform, "Transform");
    Register_Enum_String(Component::Type::eVisual, "Visual");
    Register_Enum_String(Component::Type::eMaterial, "Material");
  }
  End_Enum_String;
}