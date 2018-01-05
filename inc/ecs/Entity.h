/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include <Logger.hpp>

#include "lava.h"
#include "Component.h"

namespace lava
{
  class CEntity
  {
    SERIALIZABLE(CEntity)

  public:
    CEntity() = default;
    virtual ~CEntity() = default;

    void Update(float _dt);
    void Render();
    
    void SetName(const std::string& aName) { mName = aName; }
    const std::string& GetName() const  { return mName; }
    bool IsActive() const { return mActive; }
    void SetActive(bool aActive) { mActive = aActive; }

    const size_t GetNumberOfComponents() const
    {
      return mComponents.size();
    }

    CComponentPtr GetComponentByIdx(const size_t idx) const
    {
      return mComponents[idx];
    }

    template<typename TComponentType> std::shared_ptr<TComponentType> AddComponent()
    {
      static_assert(std::is_base_of<CComponent, TComponentType>::value);
      std::shared_ptr<TComponentType> lComponent = GetComponent<TComponentType>();
      if (lComponent != nullptr)
      {
        Log_Warning("The entity " + mName + "already has a component of type " + typeid(TComponentType).name());
      }
      else
      {
        const size_t idx = mComponents.size();
        lComponent = Handle(TComponentType);
        mComponents.emplace_back(lComponent);
        mMapComponents[lComponent->GetComponentId()] = idx;
      }
      return lComponent;
    }

    template<typename TComponentType> std::shared_ptr<TComponentType> GetComponent()
    {
      auto lIt = mMapComponents.find(EnumString<CComponent::Type>::asString(TComponentType::GetType()));
      std::shared_ptr<TComponentType> lComponent = nullptr;
      if (lIt != mMapComponents.end())
        lComponent = std::static_pointer_cast<TComponentType>(mComponents[lIt->second]);
      return lComponent;
    }

  private:
    std::unordered_map<std::string, size_t> mMapComponents;
    std::vector<CComponentPtr> mComponents;
    std::string mName;
    bool mActive = true;

    void Refresh()
    {
      for (size_t i = 0, lCount = mComponents.size(); i < lCount; ++i)
        mMapComponents[mComponents[i]->GetComponentId()] = i;
    }
  };
}