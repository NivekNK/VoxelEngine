#include "nkpch.h"

#include "Core/Rendering/Renderer.h"

#include <GLFW/glfw3.h>

namespace nk {
    Renderer::Renderer(std::shared_ptr<Window> window)
        : m_Window(std::move(window)) {
        m_Device = std::make_shared<lve::Device>(m_Window);
        
        LoadModels();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    Renderer::~Renderer() {
        vkDestroyPipelineLayout(m_Device->device(), m_PipelineLayout, nullptr);
    }

    void Renderer::LoadModels() {
        std::vector<Model::Vertex> vertices {
            {{ 0.0f,-0.5f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        
        m_Model = std::make_unique<Model>(m_Device, vertices);
    }

    void Renderer::CreatePipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(m_Device->device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            CoreError("Failed to create Pipeline Layout!");
        }
    }

    void Renderer::CreatePipeline() {
        PipelineConfigInfo pipelineConfig = {};
        Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = m_SwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );
    }

    void Renderer::RecreateSwapChain() {
        u16 width = m_Window->GetWidth();
        u16 height = m_Window->GetHeight();
        while (width == 0 || height == 0) {
            u16 width = m_Window->GetWidth();
            u16 height = m_Window->GetHeight();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_Device->device());
        m_SwapChain = std::make_unique<lve::SwapChain>(m_Device, VkExtent2D{width, height});
        CreatePipeline();
    }

    void Renderer::CreateCommandBuffers() {
        m_CommandBuffers.resize(m_SwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device->getCommandPool();
        allocInfo.commandBufferCount = static_cast<u32>(m_CommandBuffers.size());

        if (vkAllocateCommandBuffers(m_Device->device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            CoreError("Failed to allocate Command Buffers!");
        }
    }

    void Renderer::RecordCommandBuffer(i32 imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            CoreError("Failed yo begin recording Command Buffers!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<f32>(m_SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<f32>(m_SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->getSwapChainExtent()};
        vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

        m_Pipeline->Bind(m_CommandBuffers[imageIndex]);
        m_Model->Bind(m_CommandBuffers[imageIndex]);
        m_Model->Draw(m_CommandBuffers[imageIndex]);

        vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
        if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
            CoreError("Failed to record command buffer!");
        }
    }

    void Renderer::DrawFrame() {
        u32 imageIndex;
        auto result = m_SwapChain->acquireNextImage(&imageIndex);

        // A resize ocurred, need to update swap chain
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            return;
        }

        // To handle swap chain size changes
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            CoreError("Failed to acquire Swap Chain image!");
        }

        RecordCommandBuffer(imageIndex);
        result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasWindowResized()) {
            m_Window->ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS) {
            CoreError("Failed to Present Swap Chain image!");
        }
    }
} // namespace nk
