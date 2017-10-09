#pragma once

#include "lava.h"
#include "Geometry.h"

namespace lava
{
  class Renderer;
  namespace io
  {
    class Mesh
    {
    public:
      struct Vertex
      {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 uv;
      };

    public:
      Mesh() = default;
      virtual ~Mesh() = default;
      bool Load(const std::string& _filePath, Renderer& _renderer);

    public:
      std::vector< IndexedGeometry<Vertex> > mGeometry;
    };
  }
}
