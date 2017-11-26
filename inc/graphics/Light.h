#pragma once

#include "lava.h"

#include "ecs/Component.h"

namespace lava
{
  class Light: public Component
  {
  public:
    enum class Type
    {
      ePoint = 0,
      eSpot,
      eDirectional,

      MAX
    };
  public:
    Light()
      : Component(Component::Type::eLight)
    {

    }
    virtual ~Light() = default;

    Type& type() { return mType; }
    glm::vec3& position() { return mPosition; }
    glm::vec3& color() { return mColor; }
    glm::vec2& rangeAttenuation () { return mRangeAttenuation; }
    float&     intensity() { return mIntensity; }
    
  private:
    glm::vec3 mPosition;
    glm::vec3 mColor;
    glm::vec2 mRangeAttenuation;
    float mIntensity;
    Type mType;
  };

  Begin_Enum_String(Light::Type)
  {
    Register_Enum_String(Light::Type::ePoint,       "Point");
    Register_Enum_String(Light::Type::eSpot,        "Spot");
    Register_Enum_String(Light::Type::eDirectional, "Directional");
  }
  End_Enum_String;
}