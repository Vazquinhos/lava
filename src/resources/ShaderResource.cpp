#include "resources/ShaderResource.h"
#include "render/Shader.h"

namespace lava
{
    CShaderResource::CShaderResource()
        : CResource()
    {
        mResourceType = CResource::Type::eShader;
    }

    CShaderResource::~CShaderResource()
    {
    }

    bool CShaderResource::Load(const std::string& aFilename)
    {        
        SetFilename(std::string("shaders/") + aFilename);
        mResourceInternal = Handle(CShader);

        size_t shaderSize = 0;
        char* shaderCode = nullptr;
        std::ifstream is(mFilename, std::ios::binary | std::ios::in | std::ios::ate);
        if (is.is_open())
        {
          shaderSize = is.tellg();
          is.seekg(0, std::ios::beg);
          shaderCode = new char[shaderSize];
          is.read(shaderCode, shaderSize);
          is.close();
          assert(shaderSize > 0);
          mResourceInternal->Create(shaderCode, shaderSize);
          delete[] shaderCode;
          return true;
        }
        return false;
    }

    void CShaderResource::Unload()
    {
    }
}