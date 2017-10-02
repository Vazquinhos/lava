/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "vkr.hpp"

namespace vkr
{
    class MultiSample
    {
    public:
        MultiSample()
        {
            mPipelineMultisampleStateCreateInfo = {};
            mPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            mPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        }
        virtual ~MultiSample() = default;

        const VkPipelineMultisampleStateCreateInfo* GetVkPipelineMultisampleStateCreateInfo() const
        {
            return &mPipelineMultisampleStateCreateInfo;
        }

    private:
        VKR_DISABLE_COPY(MultiSample);
        VkPipelineMultisampleStateCreateInfo mPipelineMultisampleStateCreateInfo;
    };
}