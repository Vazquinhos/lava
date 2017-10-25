#pragma once

#include "lava.h"

#include "Visual.h"

namespace lava
{
  class VisualMesh : public Visual
  {
  public:
    VisualMesh() = default;
    virtual ~VisualMesh() = default;

    void create
    (
      VkDevice _device,
      VkPhysicalDevice _phyDevice,
      VkCommandPool _cmdPool,
      VkQueue _queue,
      const std::string& _meshId
    )
    {
      std::string inputfile = "compiled/" + _meshId;
      std::FILE* lMeshFile = 0;
      fopen_s(&lMeshFile, inputfile.c_str(), "rb");

      uint32_t numGeometries = 0;
      std::fread(&numGeometries, sizeof(uint32_t), 1, lMeshFile);

      for (uint32_t i = 0; i < numGeometries; ++i)
      {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        std::fread(&mNumVertices, sizeof(uint32_t), 1, lMeshFile);
        vertices.resize(mNumVertices);

        std::fread(vertices.data(), sizeof(Vertex), mNumVertices, lMeshFile);

        std::fread(&mNumIndices, sizeof(uint32_t), 1, lMeshFile);
        indices.resize(mNumIndices);

        std::fread(indices.data(), sizeof(uint32_t), mNumIndices, lMeshFile);

        std::shared_ptr< IndexedGeometry<Vertex, uint32_t > > geom = std::make_shared< IndexedGeometry<Vertex, uint32_t > >();
        geom->create(_device, _phyDevice, _cmdPool, _queue, vertices, indices);
        mGeometries.push_back(geom);
      }

      // Axis aligned bounding box
      glm::vec3 min, max;
      std::fread(&min, sizeof(float), 3, lMeshFile);
      std::fread(&max, sizeof(float), 3, lMeshFile);

      mAABB.create(min, max);
    }

    virtual void render(VkCommandBuffer _commandBuffer)
    {
      vkCmdDrawIndexed(_commandBuffer, mNumIndices, 1, 0, 0, 0);
    }

  private:
    uint32_t mNumVertices = 0;
    uint32_t mNumIndices = 0;
  };
}