#if 1
#include "Window.h"
#include "Renderer.h"
#include "Debug.h"
#include <iostream>

#include "io/Mesh.h"

#include "Pipeline.h"

#include "Shader.h"
#include "Technique.h"

namespace
{
  struct Vertex
  {
    float position[3];
    float uv[2];
  };

  struct UniformBufferObject {
	  glm::mat4 model;
	  glm::mat4 view;
	  glm::mat4 proj;
  };

  VkBuffer uniformBuffer;
  VkDeviceMemory uniformBufferMemory;

  lava::Pipeline pipeline;
  lava::Geometry<Vertex> triangle;
}

int main()
{
  lava::Window window("window", 800, 600);
  std::vector< std::string > extensions;
  std::vector< std::string > layers;
  lava::setOutputMessageFunction
  (
    [](lava::MessageSeverity _severity, const std::string& _msg)
  {
    std::string fullMsg = lava::to_string(_severity) + " : " + _msg + "\n";
    OutputDebugStringA(fullMsg.c_str());
  }
  );
  lava::Renderer lavaRenderer(&window, extensions, layers, [](VkCommandBuffer commandBuffer)
  {
    pipeline.Bind(commandBuffer);
	
	triangle.mVertexBuffer.Bind(commandBuffer);
	triangle.mIndexBuffer.Bind(commandBuffer);

	triangle.DrawIndexed(commandBuffer);
  });

  lava::Shader triangleVs(lavaRenderer.GetDevice(), VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv");
  lava::Shader triangleFs(lavaRenderer.GetDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv");

  lava::Technique triangleTechnique;
  triangleTechnique
    .shader(VK_SHADER_STAGE_VERTEX_BIT, &triangleVs)
    .shader(VK_SHADER_STAGE_FRAGMENT_BIT, &triangleFs);
  
  glm::vec3 colorRed(1, 0, 0);
  lava::UniformBuffer<glm::vec3> colorBuffer;

  colorBuffer
	  .renderer(lavaRenderer)
	  .create()
	  .data(&colorRed);

  pipeline
	  .renderer(lavaRenderer)
	  .flags(lava::VertexFlags::ePosition | lava::VertexFlags::eUv)
	  .technique(&triangleTechnique)
	  .topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	  .viewport(lava::Viewport(VkOffset2D({ 0,0 }), { lavaRenderer.GetFrameBufferWidth(), lavaRenderer.GetFrameBufferHeight() }));

  pipeline.mUniformBuffers.push_back(&colorBuffer);
  pipeline.Create();

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

  triangle
	  .renderer(lavaRenderer)
	  .vertices(vertices, 8)
	  .indices(indices, 36);

  while (!window.IsClosed())
  {
	  colorBuffer.data(&colorRed);
	  lavaRenderer.Update();
  }

  return 0;
}
#endif