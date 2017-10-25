#pragma once

#include "lava.h"

namespace lava
{
    class Shader
    {
    public:
        Shader(VkDevice _device, VkShaderStageFlagBits _stage, const void* _shaderContents, const size_t _size )
            : mVkShaderModule(VK_NULL_HANDLE)
            , mDevice(_device)
            , mStage(_stage)
        {
          if (!Load(_shaderContents, _size))
            errorLog("Error: Could not load shader");
        }

        Shader(VkDevice _device, VkShaderStageFlagBits _stage, const char* _shaderFilename)
            : mVkShaderModule(VK_NULL_HANDLE)
            , mDevice(_device)
            , mStage(_stage)
        {
            size_t shaderSize = 0;
            char* shaderCode = nullptr;

            std::ifstream is(_shaderFilename, std::ios::binary | std::ios::in | std::ios::ate);
            if (is.is_open())
            {
                shaderSize = is.tellg();
                is.seekg(0, std::ios::beg);
                shaderCode = new char[shaderSize];
                is.read(shaderCode, shaderSize);
                is.close();
                assert(shaderSize > 0);
            }

            if (!Load(shaderCode, shaderSize))
              errorLog( std::string("Could not open shader file \"" ) + _shaderFilename + "\"");

            delete[] shaderCode;
        }

        virtual ~Shader()
        {
            vkDestroyShaderModule(mDevice, mVkShaderModule, nullptr);
        }

        VkShaderModule GetVkShaderModule() const
        {
            return mVkShaderModule;
        }

        VkShaderStageFlagBits GetVkShaderStageFlagBits() const
        {
            return mStage;
        }

        VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo()
        {
            VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
            pipelineShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pipelineShaderStageCreateInfo.module = mVkShaderModule;
            pipelineShaderStageCreateInfo.pName  = "main";
            pipelineShaderStageCreateInfo.stage  = mStage;

            return pipelineShaderStageCreateInfo;
        }

    private:

        VkShaderModule        mVkShaderModule = VK_NULL_HANDLE;
        VkDevice              mDevice = VK_NULL_HANDLE;
        VkShaderStageFlagBits mStage  = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

        bool Load(const void* shaderContents, const size_t size)
        {
            // Create a new shader module that will be used for pipeline creation
            assert(mDevice != VK_NULL_HANDLE);
            VkShaderModuleCreateInfo moduleCreateInfo{};
            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.codeSize = size;
            moduleCreateInfo.pCode = static_cast<const uint32_t*> (shaderContents);
            return vkCreateShaderModule(mDevice, &moduleCreateInfo, nullptr, &mVkShaderModule) == VK_SUCCESS;
        }
    };
}