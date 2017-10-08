/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "lava.h"

namespace lava
{
    class Viewport
    {
    public:
		Viewport(VkOffset2D _offset2D = VkOffset2D({ 0,0 }), VkExtent2D _viewportSize = VkExtent2D({800, 600}), float _minDepth = 0.0f, float _maxDepth = 1.0f)
        {
            mViewport = {};
            mViewport.height = static_cast<float> (_viewportSize.height);
            mViewport.width = static_cast<float> (_viewportSize.width);
            mViewport.x = static_cast<float> (_offset2D.x);
            mViewport.y = static_cast<float> (_offset2D.y );
            mViewport.minDepth = _minDepth;
            mViewport.maxDepth = _maxDepth;

            mScissorRect = {};
            mScissorRect.extent = _viewportSize;
            mScissorRect.offset.x = 0;
            mScissorRect.offset.y = 0;

            mPipelineViewportStateCreateInfo = {};
            mPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            mPipelineViewportStateCreateInfo.viewportCount = 1;
            mPipelineViewportStateCreateInfo.pViewports = &mViewport;
            mPipelineViewportStateCreateInfo.scissorCount = 1;
            mPipelineViewportStateCreateInfo.pScissors = &mScissorRect;
        }
        virtual ~Viewport() = default;

        const VkPipelineViewportStateCreateInfo* GetVkPipelineViewportStateCreateInfo() const
        {
            return &mPipelineViewportStateCreateInfo;
        }

    private:
        VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo;
        VkViewport                        mViewport;
        VkRect2D                          mScissorRect;
    };
}