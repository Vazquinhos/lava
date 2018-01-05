#include "ecs/World.h"

namespace lava
{
  void CWorld::Update(float _dt)
  {
    for (CEntityPtr lEntity : mEntities)
      lEntity->Update(_dt);
  }

  void CWorld::Render()
  {
    for (CEntityPtr lEntity : mEntities)
      lEntity->Render();
  }
}