/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  class Component : public std::enable_shared_from_this<Component>
  {
  public:
    enum class Type
    {
      eTransform = 0,
      eCamera,
      eLight,
      eVisual,
      eMaterial,

      MAX
    };
  public:
    Component(const Type& _type)
      : mType(_type)
    {

    }

    virtual ~Component() = default;

    const Type& type() const { return mType; }

  private:

    Type mType;
  };

  Begin_Enum_String(Component::Type)
  {
    Register_Enum_String(Component::Type::eTransform, "Transform");
    Register_Enum_String(Component::Type::eVisual, "Visual");
    Register_Enum_String(Component::Type::eMaterial, "Material");
  }
  End_Enum_String;
}