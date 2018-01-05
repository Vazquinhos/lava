#pragma once

#include "lava.h"

#include "ecs/Component.h"

namespace lava
{
  class Light: public CComponent
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
    Light() = default;
    virtual ~Light() = default;

    static CComponent::Type GetType() { return CComponent::Type::eLight; }
    virtual std::string GetComponentId() const { return std::string("Light"); }

    Type& type() { return mType; }
    glm::vec3& color() { return mColor; }
    glm::vec2& rangeAttenuation () { return mRangeAttenuation; }
    float&     intensity() { return mIntensity; }
    
  private:
    glm::vec3 mColor = glm::vec3(1);
    glm::vec2 mRangeAttenuation = glm::vec2(0.0f, 1000.0f);
    float mIntensity = 1.0f;
    Type mType = Type::ePoint;
  };

  Begin_Enum_String(Light::Type)
  {
    Register_Enum_String(Light::Type::ePoint,       "Point");
    Register_Enum_String(Light::Type::eSpot,        "Spot");
    Register_Enum_String(Light::Type::eDirectional, "Directional");
  }
  End_Enum_String;
}