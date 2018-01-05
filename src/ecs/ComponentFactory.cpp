#include "ecs/ComponentFactory.h"

#include "ecs/Component.h"

#include "graphics/Transform.h"
#include "graphics/Camera.h"
#include "graphics/Light.h"

namespace lava
{
  CComponentFactory::CComponentFactory()
  {
    registerCreator<Transform>();
    registerCreator<CCamera>();
    registerCreator<Light>();
  }
}