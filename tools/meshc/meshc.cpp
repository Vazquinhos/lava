#include <string>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

namespace fs = std::experimental::filesystem;

struct Vertex
{
  aiVector3D position;
  aiVector3D normal;
  //float tangent[3];
  //float bitangent[3];
  aiVector2D uv;

  bool operator==(const Vertex& other) const {
    return position == other.position && normal == other.normal && uv == other.uv;
  }
};

void compileMesh(const std::string& _meshPath)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(_meshPath,
    aiProcess_CalcTangentSpace |
    aiProcess_GenNormals |
    aiProcess_GenSmoothNormals |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_FlipUVs);

  if (scene)
  {
    std::FILE* lMeshFile = 0;
    std::string newFile = std::string("compiled/") + _meshPath;
    fopen_s(&lMeshFile, newFile.c_str(), "wb");

    aiVector3D min( std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() , std::numeric_limits<float>::infinity() );
    aiVector3D max( -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() , -std::numeric_limits<float>::infinity() );

    std::fwrite(&scene->mNumMeshes, sizeof(uint32_t), 1, lMeshFile);

    std::cout << "[" << __TIME__ << "]\tNumber of meshes: " << scene->mNumMeshes << std::endl;

    for (uint32_t m = 0; m < scene->mNumMeshes; ++m)
    {
      std::vector< Vertex > vertexBuffer;
      std::vector<uint32_t> indexBuffer;

      for (uint32_t v = 0; v < scene->mMeshes[m]->mNumVertices; v++)
      {
        Vertex vertex = {};
        vertex.position = scene->mMeshes[m]->mVertices[v];

        std::swap(vertex.position.y, vertex.position.z);

        vertex.normal = scene->mMeshes[m]->mNormals[v];

        std::swap(vertex.normal.y, vertex.normal.z);

        if (scene->mMeshes[m]->HasTextureCoords(0))
        {
          vertex.uv.x = scene->mMeshes[m]->mTextureCoords[0][v].x;
          vertex.uv.y = scene->mMeshes[m]->mTextureCoords[0][v].y;
        }

        // Compute bounding box
        min.x = std::min(min.x, vertex.position.x);
        min.y = std::min(min.y, vertex.position.y);
        min.z = std::min(min.z, vertex.position.z);

        max.x = std::max(max.x, vertex.position.x);
        max.y = std::max(max.y, vertex.position.y);
        max.z = std::max(max.z, vertex.position.z);

        vertexBuffer.push_back(vertex);
      }

      uint32_t numVertices = static_cast<uint32_t>(vertexBuffer.size());
      std::fwrite(&numVertices, sizeof(uint32_t), 1, lMeshFile);
      std::fwrite(vertexBuffer.data(), sizeof(Vertex), numVertices, lMeshFile);

      for (uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
      {
        // We assume that all faces are triangulated
        for (uint32_t i = 0; i < 3; i++)
        {
          indexBuffer.push_back(scene->mMeshes[m]->mFaces[f].mIndices[i]);
        }
      }

      uint32_t numIndices = static_cast<uint32_t>(indexBuffer.size());
      std::fwrite(&numIndices, sizeof(uint32_t), 1, lMeshFile);
      std::fwrite(indexBuffer.data(), sizeof(uint32_t), numIndices, lMeshFile);

      std::cout << "[" << __TIME__ << "]\t" << "[" << m << "]Vertices: " << numVertices << " Indices: " << numIndices << std::endl;
    }

    std::fwrite(&min.x, sizeof(float), 3, lMeshFile);
    std::fwrite(&max.x, sizeof(float), 3, lMeshFile);

    std::fclose(lMeshFile);
  }
}

int main(int argc, char* argv[])
{
  // Check the number of parameters
  if (argc < 3) {
    // Tell the user how to run the program
    std::cerr << "Usage: " << argv[0] << " meshc -p path" << std::endl;
    /* "Usage messages" are a conventional way of telling the user
    * how to run a program if they enter the command incorrectly.
    */
    return 1;
  }

  std::cout << "***** Lava Mesh Compiler *****" << std::endl;
  fs::create_directories("compiled/meshes/");
  for (auto & p : fs::directory_iterator(argv[2]))
  {
    std::cout << std::endl << "[" << __TIME__ << "]Compiling: " << p << std::endl;
    std::ostringstream oss;
    oss << p;
    compileMesh(oss.str());
  }

  return 0;
}