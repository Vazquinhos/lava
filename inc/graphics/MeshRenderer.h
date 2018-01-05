#pragma once

#include "lava.h"

#include "ecs/Component.h"

namespace lava
{
  class CMeshRenderer : public CComponent
  {
    SERIALIZABLE(CMeshRenderer)

  public:
    CMeshRenderer() = default;
    virtual ~CMeshRenderer() = default;

    static CComponent::Type GetType() { return CComponent::Type::eVisual; }
    virtual std::string GetComponentId() const { return std::string("Visual"); }

    void SetMeshId(const std::string& aMeshId) { mMeshId = aMeshId; }
    const std::string& GetMeshId() const { return mMeshId; }

    CMeshPtr GetMesh() const { return mMesh; }

  private:
    std::string mMeshId;
    CMeshPtr mMesh = nullptr;
  };
}