#pragma once

#include "Core/Window.h"

#include "Core/Rendering/Device.h"
#include "Core/Rendering/Pipeline.h"
#include "Core/Rendering/SwapChain.h"
#include "Core/Rendering/Model.h"

namespace nk {
    class Renderer {
    public:
        Renderer(std::shared_ptr<Window> window);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void DrawFrame();
        void WaitEnd() { vkDeviceWaitIdle(m_Device->device()); }

    private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void RecreateSwapChain();
        void RecordCommandBuffer(i32 imageIndex);

        std::shared_ptr<Window> m_Window;
        std::shared_ptr<lve::Device> m_Device;
        std::unique_ptr<lve::SwapChain> m_SwapChain;
        std::unique_ptr<Pipeline> m_Pipeline;

        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::unique_ptr<Model> m_Model;
    };
} // namespace nk