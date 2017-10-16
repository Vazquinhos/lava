#include "io/Mesh.h"
#include "Debug.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace lava
{
  namespace io
  {
    bool Mesh::Load(const std::string& _filePath, Renderer& _renderer)
    {
      // Create an instance of the Importer class
      Assimp::Importer importer;
      // And have it read the given file with some example postprocessing
      // Usually - if speed is not the most important aspect for you - you'll
      // probably to request more postprocessing than we do in this example.
      const aiScene* scene = importer.ReadFile(_filePath,
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_GenSmoothNormals |
        aiProcess_Triangulate |
        aiProcess_FlipWindingOrder |
        aiProcess_FlipUVs);

      if (scene)
      {
        // Generate vertex buffer from ASSIMP scene data
        float scale = 1.0f;
        
        // Iterate through all meshes in the file and extract the vertex components
        uint32_t numMeshes = scene->mNumMeshes;
        mGeometry.resize(numMeshes);
        uint32_t indexBase = 0;
        for (uint32_t m = 0; m < numMeshes; ++m)
        {
          std::vector<Vertex> vertexBuffer;
          std::vector<uint32_t> indexBuffer;

          for (uint32_t v = 0; v < scene->mMeshes[m]->mNumVertices; ++v)
          {
            Vertex vertex;

            // Use glm make_* functions to convert ASSIMP vectors to glm vectors
            vertex.position = glm::make_vec3(&scene->mMeshes[m]->mVertices[v].x) * scale;
            vertex.normal = glm::make_vec3(&scene->mMeshes[m]->mNormals[v].x);

            if(scene->mMeshes[m]->HasTextureCoords(v) )
            {
              // Texture coordinates and colors may have multiple channels, we only use the first [0] one
              vertex.uv = glm::make_vec2(&scene->mMeshes[m]->mTextureCoords[0][v].x);
            }
            

            // Vulkan uses a right-handed NDC (contrary to OpenGL), so simply flip Y-Axis
            vertex.position.y *= -1.0f;

            vertexBuffer.push_back(vertex);
          }

          for (uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
          {
            // We assume that all faces are triangulated
            for (uint32_t i = 0; i < 3; i++)
            {
              indexBuffer.push_back(scene->mMeshes[m]->mFaces[f].mIndices[i] + indexBase);
            }
          }

          indexBase += static_cast<uint32_t>(indexBuffer.size());

          mGeometry[m]
            .renderer(_renderer)
            .vertices(vertexBuffer.data(), static_cast<uint32_t>(vertexBuffer.size()))
            .indices(indexBuffer.data(), static_cast<uint32_t>(indexBuffer.size()));
        }

        return true;
      }

      return false;
    }
  }
}
