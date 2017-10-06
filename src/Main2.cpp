#include "Window.h"
#include "Renderer.h"
#include "Debug.h"
#include <iostream>

#include "io/Mesh.h"

#include "Pipeline.h"
#include "Shader.h"
#include "Technique.h"

#include <chrono>

namespace
{
  std::shared_ptr< lava::Pipeline > mPipeline = nullptr;
  struct Vertex
  {
    float position[3];
    float uv[2];
  };

  std::shared_ptr< lava::Geometry<Vertex> > mGeometry = nullptr;
}

int main2()
{
  lava::Window window("window", 400, 300);
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
    mPipeline->Bind(commandBuffer);
    mGeometry->DrawIndexed(commandBuffer);
  });

  lava::Shader triangleVs(lavaRenderer.GetDevice(), VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv"); 
  lava::Shader triangleFs(lavaRenderer.GetDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv");

  lava::Technique triangleTechnique(triangleVs, triangleFs);

  struct PerObjectUniformBuffer
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  PerObjectUniformBuffer ubo;
  lava::UniformBuffer<PerObjectUniformBuffer> uniformBuffer(lavaRenderer);

  mPipeline = std::make_shared< lava::Pipeline>(lavaRenderer);
  mPipeline->SetInputVertexLayout(0, lava::VertexFlag::Position, lava::VertexFlag::Uv);
  mPipeline->SetTechnique(&triangleTechnique);

  Vertex vertices[3] =
  {
    { { 1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f } },
    { { -1.0f,  1.0f, 0.0f },{ 0.0f, 1.0f } },
    { { 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f } }
  };

  uint32_t indices[3] =
  {
    0, 1, 2
  };

  mGeometry = std::make_shared< lava::Geometry<Vertex> >(lavaRenderer, vertices, 3, indices, 3);

  
  
  //lava::io::Mesh objMesh;
  //objMesh.Load("meshes/cylinder.obj", lavaRenderer);
  while (!window.IsClosed())
  {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), window.GetWidth() / (float)window.GetHeight(), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    uniformBuffer.Map();
    uniformBuffer.Copy(&ubo);
    uniformBuffer.Unmap();
    lavaRenderer.Update();
  }

  return 0;
}
