/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */

#pragma once

#include "lava.h"
#include "Entity.h"

#include <Singleton.hpp>

namespace lava
{
  class CWorld : public Singleton<CWorld>
  {
    SERIALIZABLE(CWorld)

  public:
    CWorld() = default;
    virtual ~CWorld() = default;

    void Update(float _dt);
    void Render();

    const size_t GetNumberOfEntities() const
    {
      return mEntities.size();
    }

    CEntityPtr GetEntityByIdx(const size_t idx) const
    {
      return mEntities[idx];
    }

    void AddEntity(CEntityPtr aEntity)
    {
      if (aEntity != nullptr)
      {
        const size_t idx = mEntities.size();
        mEntities.emplace_back(aEntity);
        mMapEntities[aEntity->GetName()] = idx;
      }
    }

    CEntityPtr GetEntity(const std::string& aName)
    {
      auto lIt = mMapEntities.find(aName);
      if (lIt != mMapEntities.end())
        return mEntities[lIt->second];
      return nullptr;
    }

  private:
    std::unordered_map<std::string, size_t> mMapEntities;
    std::vector<CEntityPtr> mEntities;

    void Refresh()
    {
      for (size_t i = 0, lCount = mEntities.size(); i < lCount; ++i)
        mMapEntities[mEntities[i]->GetName()] = i;
    }
  };
}