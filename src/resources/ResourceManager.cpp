#include "resources/ResourceManager.h"

#include "resources/Resource.h"
#include "resources/TextureResource.h"
#include "resources/MeshResource.h"
#include "resources/MaterialResource.h"
#include "resources/ShaderResource.h"

#include <Logger.hpp>

namespace lava
{
  CResourceManager::CResourceManager()
  {
    // Register game resources classes here!
    Register<CTextureResource, CTexture>();
    Register<CMeshResource, CMesh>();
    Register<CMaterialResource, CMaterial>();
    Register<CShaderResource, CShader>();
    //
  }

  CResourceManager::~CResourceManager()
  {
#ifdef _DEBUG
    // Only in debug show the alive resources that have not been released correctly
    DumpAliveResources();
#endif
    mResourceHolders.clear();
  }

  void CResourceManager::DumpAliveResources() const
  {
    for (auto& resourceTypeHolder : mResourceHolders)
    {
      if (resourceTypeHolder.second->GetTotalBytesSize())
      {
        Log_Warning("-----------");
        Log_Warning(resourceTypeHolder.first.c_str());
        resourceTypeHolder.second->DumpAliveResources();
        Log_Warning("-----------");
      }
    }
  }

  void CResourceManager::ResourceTypeHolder::DumpAliveResources() const
  {
    for (auto& resource : mResources)
    {
      CResourcePtr res = resource.second;
      Log_Warning(res->GetFilename());
    }
  }

  void CResourceManager::ResourceTypeHolder::AddResource(const std::string& name, CResourcePtr resource)
  {
    lavaAssert(resource && !name.empty(), "Invalid resource to be added");

    mTotalBytesSize += resource->GetBytesSize();
    mResources[name] = resource;
  }

  CResourcePtr  CResourceManager::ResourceTypeHolder::GetResource(const std::string& name) const
  {
    lavaAssert(!name.empty(), "Invalid name");

    CResourcePtr resource = nullptr;
    std::map< const std::string, CResourcePtr >::const_iterator find = mResources.find(name);
    if (find != mResources.end())
    {
      resource = find->second;
    }
    return resource;
  }

  void CResourceManager::ResourceTypeHolder::ReleaseResource(const std::string& name)
  {
    lavaAssert(!name.empty(), "Invalid name");

    std::map< const std::string, CResourcePtr >::const_iterator find = mResources.find(name);
    if (find != mResources.end())
    {
      CResourcePtr resource = find->second;

      // If the number of references reach 0, this will mean that the resource is not needed
      // anymore, therefore the total storage must be updated and the resource unloaded and deleted
      //if (resource->GetRefCount() == 0)
      {
        mTotalBytesSize -= resource->GetBytesSize();
        resource->Unload();
        mResources.erase(find);
      }
    }
    else
    {
      Log_Warning(std::string("Unable to release resource ") + name );
    }
  }
}
