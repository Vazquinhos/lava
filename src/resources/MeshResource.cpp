#include "resources/MeshResource.h"
#include "render/Mesh.h"

#include "render/Device.h"

namespace lava
{
  CMeshResource::CMeshResource()
    : CResource()
  {
    mResourceType = CResource::Type::eMesh;
  }

  CMeshResource::~CMeshResource()
  {
  }

  bool CMeshResource::Load(const std::string& filename)
  {
    SetFilename(filename);
    mResourceInternal = Handle(CMesh);

    CDevice& lDevice = lava::CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    std::string inputfile = "compiled/meshes/" + mFilename;
    std::FILE* lMeshFile = 0;
    fopen_s(&lMeshFile, inputfile.c_str(), "rb");

    uint32_t numGeometries = 0;
    std::fread(&numGeometries, sizeof(uint32_t), 1, lMeshFile);

    for (uint32_t i = 0; i < numGeometries; ++i)
    {
      std::vector<Vertex> vertices;
      std::vector<uint32_t> indices;

      uint32_t numVertices = 0;
      uint32_t numIndices = 0;

      std::fread(&numVertices, sizeof(uint32_t), 1, lMeshFile);
      vertices.resize(numVertices);

      std::fread(vertices.data(), sizeof(Vertex), numVertices, lMeshFile);

      std::fread(&numIndices, sizeof(uint32_t), 1, lMeshFile);
      indices.resize(numIndices);

      std::fread(indices.data(), sizeof(uint32_t), numIndices, lMeshFile);

      std::shared_ptr< IndexedGeometry<Vertex, uint32_t > > geom = std::make_shared< IndexedGeometry<Vertex, uint32_t > >();
      geom->create(lLogicalDevice, lDevice.GetPhyiscalDevice(), lDevice.GetCommandPool(), lDevice.GetGraphicsQueue(), vertices, indices);
      mResourceInternal->mGeometries.push_back(geom);
    }

    // Axis aligned bounding box
    glm::vec3 min, max;
    std::fread(&min, sizeof(float), 3, lMeshFile);
    std::fread(&max, sizeof(float), 3, lMeshFile);

    mResourceInternal->mAABB.create(min, max);

    // mResourceBytesSize = //TODO
    return true;
  }

  void CMeshResource::Unload()
  {
  }
}