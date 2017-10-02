/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "vkr.hpp"

namespace vkr
{
    class Blending
    {
    public:
        Blending( bool _enabled )
        {
            mPipelineColorBlendAttachmentState = {};
            mPipelineColorBlendAttachmentState.colorWriteMask = 0xF;
            mPipelineColorBlendAttachmentState.blendEnable = _enabled ? VK_TRUE : VK_FALSE;

            mPipelineColorBlendStateCreateInfo = {};
            mPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            mPipelineColorBlendStateCreateInfo.attachmentCount = 1;
            mPipelineColorBlendStateCreateInfo.pAttachments = &mPipelineColorBlendAttachmentState;
        }
        virtual ~Blending() = default;

        const VkPipelineColorBlendStateCreateInfo* GetVkPipelineColorBlendStateCreateInfo() const
        {
            return &mPipelineColorBlendStateCreateInfo;
        }

    private:
        VKR_DISABLE_COPY(Blending);

        VkPipelineColorBlendAttachmentState mPipelineColorBlendAttachmentState;
        VkPipelineColorBlendStateCreateInfo mPipelineColorBlendStateCreateInfo;
    };
}