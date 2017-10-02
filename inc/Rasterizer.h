/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "vkr.hpp"

namespace vkr
{
    class Rasterizer
    {
    public:
        Rasterizer()
        {
            mPipelineRasterizationStateCreateInfo = {};
            mPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            mPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
            mPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
            mPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            mPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
            mPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
            mPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
            mPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
        }
        virtual ~Rasterizer() = default;

        const VkPipelineRasterizationStateCreateInfo* GetVkPipelineRasterizationStateCreateInfo() const
        {
            return &mPipelineRasterizationStateCreateInfo;
        }

    private:
        VKR_DISABLE_COPY(Rasterizer);
        vk::PipelineRasterizationStateCreateInfo mRasterizationInfo;
        VkPipelineRasterizationStateCreateInfo mPipelineRasterizationStateCreateInfo;
    };
}