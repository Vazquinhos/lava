#pragma once

#include "lava.h"
#include <map>

#include <Singleton.hpp>

namespace lava
{
  #define GetAsset(ClassName, AssetName) CResourceManager::getInstance().GetResource<ClassName##Resource, ClassName>(AssetName)
  class Resource;
  class CResourceManager : public Singleton<CResourceManager>
  {
  private:
    class ResourceTypeHolder
    {
    public:
      ResourceTypeHolder() = default;
      virtual ~ResourceTypeHolder() = default;

      CResourcePtr GetResource(const std::string& name) const;
      void      AddResource(const std::string& name, CResourcePtr resource);
      void      ReleaseResource(const std::string& name);

      uint32_t    GetTotalBytesSize() const { return mTotalBytesSize; }

      void      DumpAliveResources() const;

    protected:
      std::map< const std::string, CResourcePtr > mResources;
      uint32_t mTotalBytesSize;
    };

  public:
    CResourceManager();
    virtual ~CResourceManager();

    template <typename T, typename U>
    void Register()
    {
      static_assert(std::is_base_of<CResource, T>::value, "Registered type must derive from CResource");
      mResourceHolders[typeid(U).name()] = Handle(ResourceTypeHolder);
    }

    template< typename HOLDER , typename RESOURCE_TYPE  >
    std::shared_ptr<RESOURCE_TYPE> GetResource(const std::string& name)
    {
      lavaAssert(!name.empty(), "The name of the resource is empty");
      lavaAssert(mResourceHolders.find(typeid(RESOURCE_TYPE).name()) != mResourceHolders.end(), (std::string("Resource type not registered") + typeid(RESOURCE_TYPE).name()).c_str());

      std::shared_ptr<ResourceTypeHolder>& holder = mResourceHolders[typeid(RESOURCE_TYPE).name()];
      CResourcePtr resource = holder->GetResource(name);
      if (!resource)
      {
        resource = Handle(HOLDER);
        if (resource->Load(name))
          holder->AddResource(name, resource);
        else
          Log_Warning( std::string("Unable to load resource") + name );
      }

      return std::static_pointer_cast<HOLDER>(resource)->GetInternal();
    }

    template< typename RESOURCE_TYPE >
    void ReleaseResource(const std::string& name)
    {
      lavaAssert(!name.empty(), "The name of the resource is empty");
      lavaAssert(mResourceHolders.find(typeid(RESOURCE_TYPE).name()) != mResourceHolders.end(), (std::string("Resource type not registered") + typeid(RESOURCE_TYPE).name()).c_str());
      mResourceHolders[typeid(RESOURCE_TYPE).name()]->ReleaseResource(name);
    }

  private:

    std::map <std::string, std::shared_ptr<ResourceTypeHolder>> mResourceHolders;

    void DumpAliveResources() const;
  };
}