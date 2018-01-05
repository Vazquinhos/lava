#include "resources/MaterialResource.h"
#include "render/Material.h"

#include "render/Device.h"

namespace lava
{
  CMaterialResource::CMaterialResource()
    : CResource()
  {
    mResourceType = CResource::Type::eMaterial;
  }

  CMaterialResource::~CMaterialResource()
  {
  }

  bool CMaterialResource::Load(const std::string& filename)
  {
    SetFilename(filename);
    mResourceInternal = Handle(CMaterial);
    return true;
  }

  void CMaterialResource::Unload()
  {
  }
}