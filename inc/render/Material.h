#pragma  once

#include  "ecs/Component.h"

namespace  lava
{
  class CMaterial : public CComponent
  {
    SERIALIZABLE(CMaterial)

  public:
    enum class Channel
    {
      eAlbedo = 0,
      eNormal,
      eSpecular,
      eRoughness,
      eSelfIlum,

      MAX
    };
  public:
    CMaterial() = default;
    virtual ~CMaterial() = default;

    static CComponent::Type GetType() { return CComponent::Type::eMaterial; }
    virtual std::string GetComponentId() const { return std::string("Material"); }

    void SetTexture(Channel aChanel, CTexturePtr aTexture);
    void SetTechnique(CTechniquePtr aTechnique) { mTechnique = aTechnique; }

    template< typename T > void AddParameter(const std::string& aParameterId, T aParameter);

    void Bind(VkCommandBuffer aCommandBuffer);

    void Create();

  protected:

    struct MaterialParameter
    {
      std::string _parameterId;
      std::any _data;
    };

    std::array< CTexturePtr, static_cast<size_t>(Channel::MAX)> mTextures;
    std::vector< MaterialParameter > mParameters;
    
    CTechniquePtr mTechnique;
    
    VkDescriptorSet mDescriptorSet = nullptr;
    VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
    VkPipelineLayout mLayout = nullptr;
    VkPipeline mPipeline = nullptr;
  };

  Begin_Enum_String(CMaterial::Channel)
  {
    Register_Enum_String(CMaterial::Channel::eAlbedo, "Albedo");
    Register_Enum_String(CMaterial::Channel::eNormal, "Normal");
    Register_Enum_String(CMaterial::Channel::eSpecular, "Specular");
    Register_Enum_String(CMaterial::Channel::eRoughness, "Roughness");
    Register_Enum_String(CMaterial::Channel::eSelfIlum, "SelfIlum");
  }
  End_Enum_String;
}