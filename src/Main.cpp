#if 0
#include "Renderer.h"
#include "Debug.h"
#include <iostream>

#include "io/Mesh.h"

#include "Pipeline.h"
#include "Shader.h"
#include "Technique.h"
#include "io\Mesh.h"
#include "lava.h"
#include "Device.h"

namespace
{
  struct Vertex
  {
    float position[3];
    float normal[3];
    float uv[2];
  };

  lava::Pipeline pipeline;
  lava::IndexedGeometry<Vertex> triangle;
  lava::io::Mesh mesh;
}

INT WinMain(HINSTANCE,HINSTANCE,PSTR,INT )
{
  lava::Renderer lavaRenderer;
  lavaRenderer
    .outputMsgFunction
    (
      [](lava::MessageSeverity _severity, const std::string& _msg)
      {
        std::string fullMsg = lava::to_string(_severity) + " : " + _msg + "\n";
        OutputDebugStringA(fullMsg.c_str());
      }
    )
    .createWindow(800,600)
    .renderingFunction
    (
      [](VkCommandBuffer commandBuffer)
      {
        pipeline.Bind(commandBuffer);
        triangle.bind(commandBuffer);
        triangle.drawIndexed(commandBuffer);

        /*for (size_t i = 0; i < mesh.mGeometry.size(); ++i)
        {
          mesh.mGeometry[i].bind(commandBuffer);
          mesh.mGeometry[i].drawIndexed(commandBuffer);
        }*/
      }
    )
    .Create();

  lava::Shader triangleVs(lava::deviceInstance().vulkanDevice() , VK_SHADER_STAGE_VERTEX_BIT, "shaders/ubo/vert.spv");
  lava::Shader triangleFs(lava::deviceInstance().vulkanDevice() , VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/ubo/frag.spv");

  lava::Technique triangleTechnique;
  triangleTechnique
    .shader(VK_SHADER_STAGE_VERTEX_BIT, &triangleVs)
    .shader(VK_SHADER_STAGE_FRAGMENT_BIT, &triangleFs);

  pipeline
    .renderer(lavaRenderer)
    .flags(lava::VertexFlags::ePosition | lava::VertexFlags::eNormal | lava::VertexFlags::eUv)
    .technique(&triangleTechnique)
    .topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  pipeline.Create();

  //mesh.Load("meshes/bunny.obj", lavaRenderer);

  std::vector<Vertex> vertices = {
    { { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
    { { 0.5f, 0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } },

    { { -0.5f, -1.5f, -1.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -1.5f, -1.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
    { { 0.5f, 1.5f, -1.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
    { { -0.5f, 1.5f, -1.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } }
  };

  std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
  };

  /*
  Vertex vertices[8] =
  {
	  // front
	 {{-1.0, -1.0,  1.0 },{0,0} },
	 {{1.0, -1.0,  1.0	 },{ 0,0 } },
	 {{1.0,  1.0,  1.0	 },{ 0,0 } },
	 {{-1.0,  1.0,  1.0}	,{ 0,0 } },
	  // back
	 {{ -1.0, -1.0, -1.0}	,{ 0,0 } },
	 {{ 1.0, -1.0f, -1.0}	,{ 0,0 } },
	 {{ 1.0,  1.0, -1.0 }	,{ 0,0 } },
	 {{ -1.0,  1.0, -1.0,}	,{ 0,0 } },
	};

  uint32_t indices[36] =
  {
	  // front
	  0, 1, 2,
	  2, 3, 0,
	  // top
	  1, 5, 6,
	  6, 2, 1,
	  // back
	  7, 6, 5,
	  5, 4, 7,
	  // bottom
	  4, 0, 3,
	  3, 7, 4,
	  // left
	  4, 5, 1,
	  1, 0, 4,
	  // right
	  3, 2, 6,
	  6, 7, 3,
  };
  */

  triangle
	  .renderer(lavaRenderer)
	  .vertices(vertices.data(), vertices.size())
	  .indices(indices.data(), indices.size());

  lavaRenderer.Run();

  //triangle.destroy();
  return 0;
}
#endif