#pragma once

#include "lava.h"
#include "Singleton.hpp"

namespace lava
{
  class Device : Singleton<Device>
  {
  public:
    Device() = default;
    virtual ~Device() = default;

    void create();
    void destroy();

  private:
    VkInstance mInstance = nullptr;

  private:
    void createInstance();
  };
}
