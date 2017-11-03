#pragma  once

#include  "lava.h"

namespace  lava
{
  class Texture;
  class Techinque;
  class Material
  {
  public:
    Material() = default;
    virtual ~Material() = default;

    void addTexture(const std::string& _textureId, std::shared_ptr<Texture> _texture);

    template< typename T >
    void addParameter(const std::string& _parameterId, T _parameter);

    void bind(VkCommandBuffer commandBuffer);

    void create(VkDevice _device, VkRenderPass _renderPass, VkDescriptorPool _descriptorPool, VkExtent2D _swapChainExtent, MaterialType _type);
    
    virtual void destroy(VkDevice _device);

  protected:

    struct MaterialTexture
    {
      std::string textureId;
      std::shared_ptr<Texture> data;
    };

    struct MaterialParameter
    {
      std::string _parameterId;
      std::any _data;
    };

    VkDescriptorSetLayout mDescriptorSetLayout;
    Pipeline*             mPipeline;
    std::vector< MaterialTexture > mTextures;
    std::vector< MaterialParameter > mParameters;

    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout layout;
    VkPipeline pipeline;
  };
}