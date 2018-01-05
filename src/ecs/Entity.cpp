#include "ecs/Entity.h"

#include "render/Device.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "graphics/MeshRenderer.h"

namespace lava
{
  void CEntity::Update(float _dt)
  {
    for (CComponentPtr lCmp : mComponents)
      lCmp->Update(_dt);
  }

  void CEntity::Render()
  {
    CMaterialPtr lMaterial = GetComponent<CMaterial>();
    if (lMaterial != nullptr)
    {
      VkCommandBuffer lFrameCmdBuffer = CDevice::getInstance().GetFrameCommandBuffer();
      CMeshRendererPtr lMeshRenderer = GetComponent<CMeshRenderer>();
      
      for (size_t j = 0, lCount = lMeshRenderer->GetMesh()->GetGeometryCount(); j < lCount; ++j)
      {
        const auto& lGeometry = lMeshRenderer->GetMesh()->GetGeometryByIdx(j);
        
        lGeometry->bind(lFrameCmdBuffer);
        lMaterial->Bind(lFrameCmdBuffer);
        lGeometry->render(lFrameCmdBuffer);
      }
    }
  }
}
