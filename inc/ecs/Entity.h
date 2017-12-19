/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"
#include "Component.h"

#include "serialization/serialization.h"

namespace lava
{
  class Entity
  {
    SERIALIZABLE(Entity)

  public:
    Entity() = default;
    virtual ~Entity() = default;
    
    std::string& name() { return mName; }
    bool& active() { return mActive; }

    const size_t length()
    {
      return mComponents.size();
    }

    Component* at(const size_t idx)
    {
      return mComponents[idx].get();
    }

    template<typename TComponentType>
    TComponentType* addComponent()
    {
      TComponentType* componentRef = getComponent<TComponentType>();
      if (componentRef != nullptr)
        return componentRef;

      const size_t idx = mComponents.size();
      mComponents.emplace_back(std::make_unique<TComponentType>());
      mMapComponents[typeid(TComponentType).hash_code()] = idx;

      return static_cast<TComponentType*>(mComponents[idx].get());
    }

    template<typename TComponentType>
    TComponentType* getComponent()
    {
      Component* componentRef = nullptr;
      std::unordered_map<size_t, size_t>::const_iterator it = mMapComponents.find(typeid(TComponentType).hash_code());
      if (it != mMapComponents.end())
        componentRef = mComponents[it->second].get();
      return static_cast<TComponentType*> (componentRef);
    }
    
  private:
    std::unordered_map<size_t, size_t> mMapComponents;
    std::vector<std::unique_ptr<Component>> mComponents;
    std::string mName;
    bool mActive = true;
  };
}