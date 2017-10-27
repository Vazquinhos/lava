
#include "graphics\GraphicsManager.h"

#include "graphics/Camera.h"

namespace lava
{
  namespace
  {
    ObjectPool< Camera, 5 > mCameraPool;
  }
  
  template<>
  std::shared_ptr<Camera> NewObject<Camera>()
  {
    return mCameraPool.newObject();
  }
}