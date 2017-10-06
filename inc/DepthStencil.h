/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "lava.h"

namespace lava
{
    class DepthStencil
    {
    public:
        DepthStencil()
        {
            mPipelineDepthStencilStateCreateInfo = {};
            mPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            mPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
            mPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
            mPipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;
            mPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
            mPipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
            mPipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
            mPipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
            mPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
            mPipelineDepthStencilStateCreateInfo.front = mPipelineDepthStencilStateCreateInfo.back;
        }
        virtual ~DepthStencil() = default;

        const VkPipelineDepthStencilStateCreateInfo* GetVkPipelineDepthStencilStateCreateInfo() const
        {
            return &mPipelineDepthStencilStateCreateInfo;
        }

    private:
        VkPipelineDepthStencilStateCreateInfo mPipelineDepthStencilStateCreateInfo;
    };
}