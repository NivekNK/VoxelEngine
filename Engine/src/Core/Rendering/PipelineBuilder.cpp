#include "nkpch.h"

#include "PipelineBuilder.h"

namespace nk {
    PipelineBuilder::PipelineBuilder() {
        m_VertexInputInfo = VertexInputStateCreateInfo();
        m_MultisampleInfo = MultisampleStateCreateInfo();
        m_ColorBlendAttachment = ColorBlendAttachmentState();
    }

    PipelineBuilder::~PipelineBuilder() {}

    VkPipeline PipelineBuilder::BuildPipeline(VkDevice device, VkRenderPass renderpass) {
        VkPipelineViewportStateCreateInfo viewport_info = {};
        viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_info.pNext = nullptr;
        viewport_info.viewportCount = 1;
        viewport_info.pViewports = &m_Viewport;
        viewport_info.scissorCount = 1;
        viewport_info.pScissors = &m_Scissor;

        VkPipelineColorBlendStateCreateInfo color_blend_info = {};
        color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_info.pNext = nullptr;
        color_blend_info.logicOpEnable = VK_FALSE;
        color_blend_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_info.attachmentCount = 1;
        color_blend_info.pAttachments = &m_ColorBlendAttachment;

        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = nullptr;
        pipeline_info.stageCount = m_ShaderStages.size();
        pipeline_info.pStages = m_ShaderStages.data();
        pipeline_info.pVertexInputState = &m_VertexInputInfo;
        pipeline_info.pInputAssemblyState = &m_InputAssemblyInfo;
        pipeline_info.pViewportState = &viewport_info;
        pipeline_info.pRasterizationState = &m_RasterizationInfo;
        pipeline_info.pMultisampleState = &m_MultisampleInfo;
        pipeline_info.pColorBlendState = &color_blend_info;
        pipeline_info.layout = m_Layout;
        pipeline_info.renderPass = renderpass;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        VkPipeline new_pipeline;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &new_pipeline) != VK_SUCCESS) {
            CoreWarn("Failed to create Vulkan Pipeline.");
            return VK_NULL_HANDLE;
        } else {
            return new_pipeline;
        }
    }

    void PipelineBuilder::AddShaderStage(VkShaderStageFlagBits flag_bits, VkShaderModule shader_module) {
        m_ShaderStages.push_back(ShaderStageCreateInfo(flag_bits, shader_module));
    }

    void PipelineBuilder::SetViewport(f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth) {
        m_Viewport.x = x;
        m_Viewport.y = y;
        m_Viewport.width = width;
        m_Viewport.height = height;
        m_Viewport.minDepth = min_depth;
        m_Viewport.maxDepth = max_depth;
    }

    void PipelineBuilder::SetScissor(VkOffset2D offset, VkExtent2D extent) {
        m_Scissor.offset = offset;
        m_Scissor.extent = extent;
    }

    VkPipelineShaderStageCreateInfo PipelineBuilder::ShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader_module) {
        VkPipelineShaderStageCreateInfo shader_stage_info = {};
        shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.pNext = nullptr;
        shader_stage_info.stage = stage;
        shader_stage_info.module = shader_module;
        shader_stage_info.pName = "main";
        return shader_stage_info;
    }

    VkPipelineVertexInputStateCreateInfo PipelineBuilder::VertexInputStateCreateInfo() {
        VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.pNext = nullptr;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        return vertex_input_info;
    }

    VkPipelineInputAssemblyStateCreateInfo PipelineBuilder::InputAssemblyStateCreateInfo(VkPrimitiveTopology topology) {
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
        input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_info.pNext = nullptr;
        input_assembly_info.topology = topology;
        input_assembly_info.primitiveRestartEnable = VK_FALSE;
        return input_assembly_info;
    }

    VkPipelineRasterizationStateCreateInfo PipelineBuilder::RasterizationStateCreateInfo(VkPolygonMode polygon_mode) {
        VkPipelineRasterizationStateCreateInfo rasterization_info = {};
        rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_info.pNext = nullptr;
		rasterization_info.depthClampEnable = VK_FALSE;
		rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		rasterization_info.polygonMode = polygon_mode;
		rasterization_info.lineWidth = 1.0f;
		rasterization_info.cullMode = VK_CULL_MODE_NONE;
		rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterization_info.depthBiasEnable = VK_FALSE;
		rasterization_info.depthBiasConstantFactor = 0.0f;
		rasterization_info.depthBiasClamp = 0.0f;
		rasterization_info.depthBiasSlopeFactor = 0.0f;
		return rasterization_info;
    }

    VkPipelineMultisampleStateCreateInfo PipelineBuilder::MultisampleStateCreateInfo() {
        VkPipelineMultisampleStateCreateInfo multisample_info = {};
        multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_info.pNext = nullptr;
		multisample_info.sampleShadingEnable = VK_FALSE;
		multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_info.minSampleShading = 1.0f;
		multisample_info.pSampleMask = nullptr;
		multisample_info.alphaToCoverageEnable = VK_FALSE;
		multisample_info.alphaToOneEnable = VK_FALSE;
		return multisample_info;
    }

    VkPipelineColorBlendAttachmentState PipelineBuilder::ColorBlendAttachmentState() {
        VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT |
			                                    VK_COLOR_COMPONENT_B_BIT |
                                                VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;
		return color_blend_attachment;
    }

    VkPipelineLayoutCreateInfo PipelineBuilder::LayoutCreateInfo() {
        VkPipelineLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layout_info.pNext = nullptr;
		layout_info.flags = 0;
		layout_info.setLayoutCount = 0;
		layout_info.pSetLayouts = nullptr;
		layout_info.pushConstantRangeCount = 0;
		layout_info.pPushConstantRanges = nullptr;
		return layout_info;
    }
} // namespace nk
