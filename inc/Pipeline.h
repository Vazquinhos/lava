/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "vkr.hpp"

namespace vkr
{
    class Shader;

    class Pipeline
    {
    public:
        template<typename... VertexFlags>
        Pipeline
        (   
            VkDevice _device,
            const VkRenderPass _renderPass,
            const VkPipelineBindPoint _pipelineBindPoint,
            const VkPrimitiveTopology _topology,
            const vkr::Viewport& _viewport,
            const vkr::Blending& _blending,
            const vkr::Rasterizer& _rasterizer,
            const vkr::DepthStencil& _depthStencil,
            const vkr::MultiSample& _multisample,
            const vkr::Techinique& _technique,
            VertexFlags&&... _flags
        )
            : mDevice(_device)
            , mPipelineBindPoint(_pipelineBindPoint)
        {
            vk::Pipeline
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            if (vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout) == VK_SUCCESS)
            {
                VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
                graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                
                graphicsPipelineCreateInfo.renderPass = _renderPass;

                graphicsPipelineCreateInfo.layout = mPipelineLayout;

                static const uint32_t size = sizeof...(VertexFlags);
                std::vector< VkVertexInputAttributeDescription > vVertexInputAttributeDescription(size);
                uint32_t offset = 0;
                std::vector<VertexFlag> flags = { _flags... };
                for (uint32_t location = 0; location < size; ++location)
                {
                    vVertexInputAttributeDescription[location].binding = 0;
                    vVertexInputAttributeDescription[location].location = location;
                    vVertexInputAttributeDescription[location].offset = offset;

                    VkFormat format = VK_FORMAT_UNDEFINED;
                    switch (flags[location])
                    {
                    case VertexFlag::Position:
                        format = VK_FORMAT_R32G32B32_SFLOAT;
                        offset += sizeof(float) * 3;
                        break;
                    case VertexFlag::Uv:
                        format = VK_FORMAT_R32G32_SFLOAT;
                        offset += sizeof(float) * 2;
                        break;
                    default:
                        assert(false);
                        break;
                    }
                    vVertexInputAttributeDescription[location].format = format;
                }

                VkVertexInputBindingDescription vertexInputBindingDescription = { 0, offset, VK_VERTEX_INPUT_RATE_VERTEX };

                VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
                pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = size;
                pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vVertexInputAttributeDescription.data();
                pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
                pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

                graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;

                VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
                pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                pipelineInputAssemblyStateCreateInfo.topology = _topology;

                graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;

                graphicsPipelineCreateInfo.pViewportState = _viewport.GetVkPipelineViewportStateCreateInfo();

                graphicsPipelineCreateInfo.pColorBlendState = _blending.GetVkPipelineColorBlendStateCreateInfo();
                graphicsPipelineCreateInfo.pRasterizationState = _rasterizer.GetVkPipelineRasterizationStateCreateInfo();
                graphicsPipelineCreateInfo.pDepthStencilState = _depthStencil.GetVkPipelineDepthStencilStateCreateInfo();
                graphicsPipelineCreateInfo.pMultisampleState = _multisample.GetVkPipelineMultisampleStateCreateInfo();
                
                graphicsPipelineCreateInfo.stageCount = _technique.GetNumStages();
                graphicsPipelineCreateInfo.pStages = _technique.GetVkPipelineShaderStageCreateInfo();

                if( vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline) != VK_SUCCESS )
                    std::cerr << "Error: Could create pipeline" << std::endl;
            }
        }

        virtual ~Pipeline() = default;

        void ShutDown()
        {
            vkDestroyPipeline(mDevice, mPipeline, nullptr);
            vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
        }

        void Bind(VkCommandBuffer _commandBuffer)
        {
            vkCmdBindPipeline(_commandBuffer, mPipelineBindPoint, mPipeline);
            std::cout << "bind pipeline" << std::endl;
        }

    private:
        VKR_DISABLE_COPY(Pipeline);
        
        VkDevice            mDevice            = VK_NULL_HANDLE;
        VkPipeline          mPipeline          = VK_NULL_HANDLE;
        VkPipelineLayout    mPipelineLayout    = VK_NULL_HANDLE;
        VkPipelineBindPoint mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    };
}