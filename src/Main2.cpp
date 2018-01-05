#if 1
#include "lava.h"
#include "render/Window.h"

#include "ecs/World.h"
#include "ecs/Component.h"
#include "ecs/Entity.h"

#include "render/Device.h"
#include "render/Buffer.h"
#include "render/Geometry.h"
#include "render/Samplers.h"
#include "render/Texture.h"
#include "imgui/imgui_impl.h"
#include "imgui/imgui_bindings.h"

#include "render/Shader.h"

#include "graphics/Transform.h"

#include "render/Vertex.h"

#include "graphics/AABB.h"
#include "graphics/Light.h"
#include "graphics/Camera.h"
#include "graphics/CameraController.h"

#include "ImGuizmo.h"
#include "imgui/ImGui.h"

lava::CImGuiPtr mImGui;


static int WIDTH = 1280;
static int HEIGHT = 720;

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

UniformBufferObject ubo = {};
bool initedMatrix = false;

#include "serialization\serialization.h"

#include <cereal/archives/json.hpp>

#include "ecs/World.h"
#include "ecs/Entity.h"
#include "ecs/Component.h"

#include "serialization\ecs\Component_io.h"

#include "serialization\ecs\Entity_io.h"
#include "serialization\graphics\Camera_io.h"
#include "serialization/ecs/Entity_io.h"
#include "serialization/ecs/World_io.h"
#include "serialization/graphics/MeshRenderer_io.h"
#include "serialization/render/Material_io.h"
#include "render/Material.h"

#include "graphics/MeshRenderer.h"

class HelloTriangleApplication {
public:
  void run()
  {
    window = new lava::Window("Vulkan", WIDTH, HEIGHT);

    lava::CDevice& lDevice = lava::CDevice::getInstance();
    lDevice.Create(window->GetHWND());

    lava::CWorld& lWorld = lava::CWorld::getInstance();
    if (false)
    {
      // Main camera
      {
        lava::CEntityPtr lCamera = Handle(lava::CEntity);
        lCamera->SetName("main camera");
        lCamera->AddComponent<lava::Transform>();
        lCamera->AddComponent<lava::CCamera>();
        lWorld.AddEntity(lCamera);
      }

      // Sun light
      {
        lava::CEntityPtr lSun = Handle(lava::CEntity);
        lSun->SetName("sun");
        lSun->AddComponent<lava::Transform>();
        lWorld.AddEntity(lSun);
      }

      // Main scene object
      {
        lava::CEntityPtr lCerberusWeapon = Handle(lava::CEntity);
        lCerberusWeapon->SetName("cerebrus");
        lCerberusWeapon->AddComponent<lava::Transform>();
        lava::CMaterialPtr lMaterial = lCerberusWeapon->AddComponent<lava::CMaterial>();
        lava::CTexturePtr lTexture = Handle(lava::CTexture);
        lMaterial->SetTexture(lava::CMaterial::Channel::eAlbedo, lTexture);
        lava::CTechniquePtr lTechnique = Handle(lava::CTechnique);
        lMaterial->SetTechnique(lTechnique);
        lTechnique->SetShader(lava::CTechnique::Stage::eVertexShader, Handle(lava::CShader));
        lTechnique->SetShader(lava::CTechnique::Stage::eFragmentShader, Handle(lava::CShader));
        lava::CMeshRendererPtr lMeshRenderer = lCerberusWeapon->AddComponent<lava::CMeshRenderer>();
        lMeshRenderer->SetMeshId("cerebrus.fbx");
        lWorld.AddEntity(lCerberusWeapon);
      }

      {
        std::ofstream os("prueba1.json");
        cereal::JSONOutputArchive ar(os);
        ar(cereal::make_nvp("world", lWorld));
      }
    }

    {
      std::ifstream os("prueba1.json");
      cereal::JSONInputArchive ar(os);
      ar(cereal::make_nvp("world", lWorld));
    }

    mainLoop();

    delete window;
  }

private:
  lava::Window* window;

  void mainLoop()
  {
    lava::CWorld& lWorld = lava::CWorld::getInstance();
    lava::CDevice& lDevice = lava::CDevice::getInstance();
    while (!window->IsClosed())
    {
      window->Update();

      lWorld.Update(1.0f/60.0f);

      lava::CEntityPtr lMainCameraEntity = lWorld.GetEntity("main camera");
      lava::CCameraPtr lMainCamera = lMainCameraEntity->GetComponent<lava::CCamera>();

      lDevice.BeginFrame(*lMainCamera.get());

      ubo.view = lMainCamera->view();
      ubo.proj = lMainCamera->proj();
      ubo.model = float4x4(1);

      lDevice.GetPerFrameUBO().update(lDevice.GetLogicalDevice(), sizeof(UniformBufferObject), &ubo);

      lWorld.Render();

      lDevice.EndFrame();
    }

    lDevice.Destroy();
  }

};

INT WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
  HelloTriangleApplication app;

  try {
    app.run();
  }
  catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

#endif