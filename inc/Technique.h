/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "lava.h"
#include "Shader.h"

namespace lava
{
  class Technique
  {
  public:
    Technique(const Shader& _vs, const Shader& _fs)
      : mNumStages(2)
    {
      mvPipelineShaderStageCreateInfos.resize(2);
      mvPipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      mvPipelineShaderStageCreateInfos[0].module = _vs.GetVkShaderModule();
      mvPipelineShaderStageCreateInfos[0].pName = "main";
      mvPipelineShaderStageCreateInfos[0].stage = _vs.GetVkShaderStageFlagBits();

      mvPipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      mvPipelineShaderStageCreateInfos[1].module = _fs.GetVkShaderModule();
      mvPipelineShaderStageCreateInfos[1].pName = "main";
      mvPipelineShaderStageCreateInfos[1].stage = _fs.GetVkShaderStageFlagBits();
    }
    virtual ~Technique() = default;

    const VkPipelineShaderStageCreateInfo* GetVkPipelineShaderStageCreateInfo() const
    {
      return mvPipelineShaderStageCreateInfos.data();
    }

    uint32_t GetNumStages() const
    {
      return mNumStages;
    }

  private:
    std::vector<VkPipelineShaderStageCreateInfo> mvPipelineShaderStageCreateInfos;
    uint32_t mNumStages;
  };
}