#pragma once

#include "Core/Rendering/Device.h"

namespace nk {
    struct PipelineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        u32 subpass = 0;
    };

    class Pipeline {
    public:
        Pipeline(
            std::shared_ptr<lve::Device> device,
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo
        );
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer);

        static PipelineConfigInfo DefaultPipelineConfigInfo(u32 width, u32 height);

    private:
        static std::vector<char> ReadFile(const std::string& filepath);

        void CreateGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo
        );

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        std::shared_ptr<lve::Device> m_Device;
        VkPipeline m_GraphicsPipeline;
        VkPipeline test;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
} // namespace nk
