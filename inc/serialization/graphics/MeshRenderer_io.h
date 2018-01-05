#pragma once

#include "ecs/Component.h"
#include "graphics/MeshRenderer.h"

#include "render/Mesh.h"
#include "resources/MeshResource.h"
#include "resources/ResourceManager.h"

CEREAL_REGISTER_TYPE(lava::CMeshRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::CComponent, lava::CMeshRenderer)

namespace lava
{
  template <class Archive>
  void save(Archive & ar, const CMeshRenderer & t)
  {
    ar(cereal::make_nvp("mesh_id", t.mMeshId));
  }

  template <class Archive>
  void load(Archive & ar, CMeshRenderer & t)
  {
    ar(cereal::make_nvp("mesh_id", t.mMeshId));
    t.mMesh = GetAsset(CMesh, t.mMeshId);
  }
}
