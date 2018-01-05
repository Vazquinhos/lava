/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"

namespace lava
{
  class CComponent
  {
    SERIALIZABLE(CComponent);

  public:
    enum class Type
    {
      eTransform = 0,
      eCamera,
      eLight,
      eMeshRenderer,
      eVisual,
      eMaterial,

      MAX
    };
  public:
    CComponent() = default;
    virtual ~CComponent() = default;

    virtual std::string GetComponentId() const = 0;
    virtual void Update( float ) {}
  };

  Begin_Enum_String(CComponent::Type)
  {
    Register_Enum_String(CComponent::Type::eLight, "Light");
    Register_Enum_String(CComponent::Type::eCamera, "Camera");
    Register_Enum_String(CComponent::Type::eTransform, "Transform");
    Register_Enum_String(CComponent::Type::eVisual, "Visual");
    Register_Enum_String(CComponent::Type::eMaterial, "Material");
  }
  End_Enum_String;
}