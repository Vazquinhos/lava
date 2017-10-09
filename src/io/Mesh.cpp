#include "io/Mesh.h"
#include "Debug.h"
#include "Renderer.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

namespace lava
{
  namespace io
  {
    bool Mesh::Load(const std::string& _filePath, Renderer& _renderer)
    {
      tinyobj::attrib_t attrib;
      std::vector<tinyobj::shape_t> shapes;
      std::vector<tinyobj::material_t> materials;

      std::string err;
      bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, _filePath.c_str());

      if (!err.empty())
      {
        if (ret)
        {
          warningLog(err);
        }
        else
        {
          debugLog(err);
          return false;
        }
      }

      if (ret)
      {
        // Loop over shapes
        mGeometry.resize(shapes.size());
        for (size_t s = 0; s < shapes.size(); ++s)
        {
          std::map< std::string, size_t > vertexBufferMap;
          std::vector< Vertex > vertexBuffer;
          std::vector< uint32_t > indexBuffer;
          
          // Loop over faces(polygon)
          size_t indexOffset = 0;
          // Loop over vertices in the face.
          for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
          {
            int fv = shapes[s].mesh.num_face_vertices[f];
            assert(fv == 3);

            const size_t indexOffset0 = indexOffset + 0, indexOffset1 = indexOffset + 1, indexOffset2 = indexOffset + 2;

            // Shortcuts for indices
            tinyobj::index_t idx0 = shapes[s].mesh.indices[indexOffset0];
            tinyobj::index_t idx1 = shapes[s].mesh.indices[indexOffset1];
            tinyobj::index_t idx2 = shapes[s].mesh.indices[indexOffset2];

            // Shortcuts for vertices
            glm::vec3 v0(attrib.vertices[3 * idx0.vertex_index + 0], attrib.vertices[3 * idx0.vertex_index + 1], attrib.vertices[3 * idx0.vertex_index + 2]);
            glm::vec3 v1(attrib.vertices[3 * idx1.vertex_index + 0], attrib.vertices[3 * idx1.vertex_index + 1], attrib.vertices[3 * idx1.vertex_index + 2]);
            glm::vec3 v2(attrib.vertices[3 * idx2.vertex_index + 0], attrib.vertices[3 * idx2.vertex_index + 1], attrib.vertices[3 * idx2.vertex_index + 2]);

            // Shortcuts for normals
            glm::vec3 n0 = idx0.normal_index > 0 ? glm::vec3(attrib.vertices[3 * idx0.normal_index + 0], attrib.vertices[3 * idx0.normal_index + 1], attrib.vertices[3 * idx0.normal_index + 2]) : glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 n1 = idx1.normal_index > 0 ? glm::vec3(attrib.vertices[3 * idx1.normal_index + 0], attrib.vertices[3 * idx1.normal_index + 1], attrib.vertices[3 * idx1.normal_index + 2]) : glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec3 n2 = idx2.normal_index > 0 ? glm::vec3(attrib.vertices[3 * idx2.normal_index + 0], attrib.vertices[3 * idx2.normal_index + 1], attrib.vertices[3 * idx2.normal_index + 2]) : glm::vec3(0.0f, 1.0f, 0.0f);

            // Shortcuts for texcoords
            glm::vec2 uv0 = idx0.texcoord_index > 0 ? glm::vec2(attrib.texcoords[2 * idx0.texcoord_index + 0], attrib.texcoords[2 * idx0.texcoord_index + 1]) : glm::vec2(1.0f, 1.0f);
            glm::vec2 uv1 = idx1.texcoord_index > 0 ? glm::vec2(attrib.texcoords[2 * idx1.texcoord_index + 0], attrib.texcoords[2 * idx1.texcoord_index + 1]) : glm::vec2(0.0f, 1.0f);
            glm::vec2 uv2 = idx2.texcoord_index > 0 ? glm::vec2(attrib.texcoords[2 * idx2.texcoord_index + 0], attrib.texcoords[2 * idx2.texcoord_index + 1]) : glm::vec2(1.0f, 0.0f);

            // Edges of the triangle : postion delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

            Vertex vertices[3] = 
            {
              { v0, n0, tangent, bitangent, uv0 },
              { v1, n1, tangent, bitangent, uv1 },
              { v2, n2, tangent, bitangent, uv2 },
            };

            for (size_t v = 0; v < fv; ++v)
            {
              std::stringstream stream;
              stream << vertices[v].position.x << " " << vertices[v].position.y << " " << vertices[v].position.z;
              stream << vertices[v].normal.x << " " << vertices[v].normal.y << " " << vertices[v].normal.z;
              stream << vertices[v].uv.x << " " << vertices[v].uv.y;
              std::map< std::string, size_t >::iterator it = vertexBufferMap.find(stream.str());
              if (it == vertexBufferMap.end())
              {
                vertexBufferMap[stream.str()] = vertexBuffer.size();
                indexBuffer.push_back((uint32_t)vertexBuffer.size());
                vertexBuffer.push_back(vertices[v]);
              }
              else
              {
                const size_t& idx = it->second;
                vertexBuffer[idx].tangent   += vertices[v].tangent;
                vertexBuffer[idx].bitangent += vertices[v].bitangent;
                indexBuffer.push_back((uint32_t)idx);
              }
            }

            indexOffset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
          }

          // Post process the tangent and bitangent
          for (size_t v = 0, count = vertexBuffer.size(); v < count; ++v)
          {
            // Gram-Schmidt orthogonalize
            vertexBuffer[v].tangent = glm::normalize(vertexBuffer[v].tangent - vertexBuffer[v].normal* glm::dot(vertexBuffer[v].normal, vertexBuffer[v].tangent));

            // Calculate handedness
            if (glm::dot(glm::cross(vertexBuffer[v].normal, vertexBuffer[v].tangent), vertexBuffer[v].bitangent) < 0.0f)
              vertexBuffer[v].tangent *= -1.0f;

            vertexBuffer[v].bitangent = glm::normalize(glm::cross(vertexBuffer[v].normal, vertexBuffer[v].tangent));
            vertexBuffer[v].normal = glm::normalize(vertexBuffer[v].normal);
          }

          mGeometry[s]
            .renderer(_renderer)
            .vertices(vertexBuffer.data(), (uint32_t)vertexBuffer.size())
            .indices(indexBuffer.data(), (uint32_t)indexBuffer.size());
        }

        return true;
      }

      return false;
    }
  }
}
