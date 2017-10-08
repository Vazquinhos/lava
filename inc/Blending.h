/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "lava.h"

namespace lava
{
    class Blending
    {
    public:
        Blending()
        {
            mPipelineColorBlendAttachmentState = {};
            mPipelineColorBlendAttachmentState.colorWriteMask = 0xF;
			mPipelineColorBlendAttachmentState.blendEnable = VK_FALSE;//_enabled ? VK_TRUE : VK_FALSE;

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
        VkPipelineColorBlendAttachmentState mPipelineColorBlendAttachmentState;
        VkPipelineColorBlendStateCreateInfo mPipelineColorBlendStateCreateInfo;
    };
}