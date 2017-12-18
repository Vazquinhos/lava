/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

#include "serialization/serialization.h"

namespace lava
{
  class Component
  {
    SERIALIZABLE(Component);

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

    Component()
      : mType(Type::eTransform)
    {

    }
    virtual ~Component() = default;

    const Type& type() const { return mType; }

  private:
    Type mType;

    Component& operator=(const Component&) = delete;  // Disallow copying
    Component(const Component&) = delete;
  };

  Begin_Enum_String(Component::Type)
  {
    Register_Enum_String(Component::Type::eLight, "Light");
    Register_Enum_String(Component::Type::eCamera, "Camera");
    Register_Enum_String(Component::Type::eTransform, "Transform");
    Register_Enum_String(Component::Type::eVisual, "Visual");
    Register_Enum_String(Component::Type::eMaterial, "Material");
  }
  End_Enum_String;
}