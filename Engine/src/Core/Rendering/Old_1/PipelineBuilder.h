#pragma once

#include <vulkan/vulkan.h>

namespace nk {
    class PipelineBuilder {
    public:
        PipelineBuilder();
        ~PipelineBuilder();

        VkPipeline BuildPipeline(VkDevice device, VkRenderPass renderpass);

        void AddShaderStage(VkShaderStageFlagBits flag_bits, VkShaderModule shader_module);
        void SetInputAssemblyInfo(VkPrimitiveTopology topology) { m_InputAssemblyInfo = InputAssemblyStateCreateInfo(topology); }
        void SetViewport(f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth);
        void SetScissor(VkOffset2D offset, VkExtent2D extent);
        void SetRasterizationInfo(VkPolygonMode polygon_mode) { m_RasterizationInfo = RasterizationStateCreateInfo(polygon_mode); }
        void SetPipelineLayout(VkPipelineLayout layout) { m_Layout = layout; }
        VkPipelineLayoutCreateInfo LayoutCreateInfo();

    private:
        VkPipelineShaderStageCreateInfo ShaderStageCreateInfo(VkShaderStageFlagBits flag_bits, VkShaderModule shader_module);
        VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo();
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo(VkPrimitiveTopology topology);
        VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygon_mode);
        VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo();
        VkPipelineColorBlendAttachmentState ColorBlendAttachmentState();

        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
        VkPipelineVertexInputStateCreateInfo m_VertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyInfo;
        VkViewport m_Viewport;
        VkRect2D m_Scissor;
        VkPipelineRasterizationStateCreateInfo m_RasterizationInfo;
        VkPipelineColorBlendAttachmentState m_ColorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo m_MultisampleInfo;
        VkPipelineLayout m_Layout;
    };
} // namespace nk
