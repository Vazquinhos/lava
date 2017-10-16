#pragma once

#include "lava.h"

#include "Debug.h"

#include "Window.h"

#include "Device.h"
#include "SwapChain.h"

namespace lava
{
  class Renderer
  {
  public:
    Renderer() = default;
    virtual ~Renderer();

    void Update();
    void Run()
    {
      while (!mWindow->IsClosed())
        Update();
    }

    void Create();

    Renderer& createWindow(const uint32_t _w, const uint32_t _h)
    {
      mWindow = new Window("Vulkan", _w, _h);
      deviceInstance().targetExtend(VkExtent2D({ _w,_h }));
      return *this;
    }

    Renderer& renderingFunction(std::function<void(VkCommandBuffer)> _renderingFnc)
    {
      deviceInstance().bindingBufferFunction = _renderingFnc;
      return *this;
    }

    Renderer& outputMsgFunction(std::function<void(MessageSeverity, const std::string&)> _outputFnc)
    {
      setOutputMessageFunction(_outputFnc);
      return *this;
    }

  protected:

    Window *mWindow = nullptr;
  };

}
