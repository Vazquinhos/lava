#include "Renderer.h"
#include "Debug.h"
#include "Window.h"

namespace lava
{
  void Renderer::Create()
  {
    setWindowHandle(mWindow->GetHWND());
    deviceInstance()
      .enableValidation(true)
      .windowHandle(mWindow->GetHWND())
      .create();
  }

  Renderer::~Renderer()
  {
   
  }

  void Renderer::Update()
  {
    mWindow->Update();
    deviceInstance().drawFrame();
  }
}
