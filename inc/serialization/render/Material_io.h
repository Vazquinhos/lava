#pragma once

#include "ecs/Component.h"
#include "render/Material.h"

CEREAL_REGISTER_TYPE(lava::CMaterial);
CEREAL_REGISTER_POLYMORPHIC_RELATION(lava::CComponent, lava::CMaterial)

#include "render/Texture.h"
#include "resources/TextureResource.h"
#include "resources/ResourceManager.h"

#include "serialization/render/Technique_io.h"

namespace lava
{
  namespace
  {
    struct STextureParameter
    {
      std::string mTextureId;
      std::string mTextureFilename;

      template<class Archive>
      void serialize(Archive & archive)
      {
        archive(cereal::make_nvp("channel", mTextureId), cereal::make_nvp("filename", mTextureFilename) );
      }
    };
  }

  template <class Archive>
  void save(Archive & ar, const CMaterial & t)
  {
    std::vector< STextureParameter > lTextureParameters;
    for (size_t i = 0; i < static_cast<size_t>(CMaterial::Channel::MAX); ++i)
    {
      if (t.mTextures[i])
        lTextureParameters.push_back({ EnumString<CMaterial::Channel>::asString(static_cast<CMaterial::Channel>(i)), t.mTextures[i]->GetId() });
    }

    ar(cereal::make_nvp("textures", lTextureParameters));
    ar(cereal::make_nvp("technique", t.mTechnique));
  }

  template <class Archive>
  void load(Archive & ar, CMaterial & t)
  {
    std::vector< STextureParameter > lTextureParameters;
    ar(cereal::make_nvp("textures", lTextureParameters));

    for (STextureParameter & lTextureParameter : lTextureParameters)
    {
      t.SetTexture
      (
        EnumString<CMaterial::Channel>::asEnum(lTextureParameter.mTextureId),
        GetAsset(CTexture, lTextureParameter.mTextureFilename)
      );
    }

    ar(cereal::make_nvp("technique", t.mTechnique));

    t.Create();
  }
}
