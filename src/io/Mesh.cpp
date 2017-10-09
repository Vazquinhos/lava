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
				mGeometry.resize(1);
				std::map< std::string, size_t > vertexBufferMap;
				std::vector< Vertex > vertexBuffer;
				std::vector< uint32_t > indexBuffer;
				for (size_t s = 0; s < shapes.size(); s++)
				{
					

					// Loop over faces(polygon)
					size_t index_offset = 0;
					for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
					{
						int fv = shapes[s].mesh.num_face_vertices[f];

						// Loop over vertices in the face.
						for (size_t v = 0; v < fv; v++)
						{
							std::stringstream stream;
							
							// access to vertex
							tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
							tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
							tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
							tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
							tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
							tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
							tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
							tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
							tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

							stream << vx << " " << vy << " " << vz << " ";
							stream << nx << " " << ny << " " << nz << " ";
							stream << tx << " " << ty;

							Vertex vertex = {};
							vertex.position = glm::vec3(vx, vy, vz);
							vertex.normal = glm::vec3(nx, ny, nz);
							vertex.uv = glm::vec2(tx, ty);

							std::map< std::string, size_t >::iterator it = vertexBufferMap.find(stream.str());
							if (it == vertexBufferMap.end())
							{
								vertexBufferMap[stream.str()] = vertexBuffer.size();
								indexBuffer.push_back(static_cast<uint32_t>(vertexBuffer.size()));
								vertexBuffer.push_back(vertex);
							}
							else
							{
								const size_t& idx = it->second;
								indexBuffer.push_back(static_cast<uint32_t>(it->second));
							}
						}

						index_offset += fv;

						// per-face material
						shapes[s].mesh.material_ids[f];
					}

					
				}

				mGeometry[0]
					.renderer(_renderer)
					.vertices(vertexBuffer.data(), static_cast<uint32_t>(vertexBuffer.size()))
					.indices(indexBuffer.data(), static_cast<uint32_t>(indexBuffer.size()));

				return true;
			}

			return false;
		}
	}
}
