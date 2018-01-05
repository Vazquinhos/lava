#pragma once

#include "render/Technique.h"

#include "render/Shader.h"
#include "resources/ShaderResource.h"
#include "resources/ResourceManager.h"

namespace lava
{
  namespace
  {
    struct SShaderValue
    {
      std::string mShaderStage;
      std::string mShaderFilename;

      template<class Archive>
      void serialize(Archive & archive)
      {
        archive(cereal::make_nvp("stage", mShaderStage), cereal::make_nvp("filename", mShaderFilename));
      }
    };
  }

  template <class Archive>
  void save(Archive & ar, const CTechnique & t)
  {
    std::vector< SShaderValue > lShaders;
    for (size_t i = 0; i < static_cast<size_t>(CTechnique::Stage::MAX); ++i)
    {
      if (t.mShaders[i])
        lShaders.push_back({ EnumString<CTechnique::Stage>::asString(static_cast<CTechnique::Stage>(i)), t.mShaders[i]->GetId() });
    }

    ar(cereal::make_nvp("shaders", lShaders));
  }

  template <class Archive>
  void load(Archive & ar, CTechnique & t)
  {
    std::vector< SShaderValue > lShaders;
    ar(cereal::make_nvp("shaders", lShaders));

    for (SShaderValue & lShader : lShaders)
    {
      t.SetShader
      (
        EnumString<CTechnique::Stage>::asEnum(lShader.mShaderStage),
        GetAsset(CShader, lShader.mShaderFilename)
      );
    }
  }
}
