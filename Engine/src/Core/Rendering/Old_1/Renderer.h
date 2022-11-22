#pragma once

#include <vulkan/vulkan.h>

#include <Core/Window.h>

#define VK_CHECK(expr) {                   \
    CoreAssertKeep(expr == VK_SUCCESS);    \
}

namespace nk {
    class Renderer {
    public:
        Renderer(const std::string& application_name, std::shared_ptr<Window> window);
        ~Renderer();

        void Draw();

        bool LoadShaderModule(const char* file_path, VkShaderModule* out_shader_module);

    private:
        void InitVulkan(const std::string& application_name);
        void InitSwapchain();
        void InitCommands();
        void InitDefaultRenderpass();
        void InitFramebuffers();
        void InitSyncStructures();
        void InitPipeline();

        VkCommandPoolCreateInfo CommandPoolCreateInfo(u32 queue_family_index, VkCommandPoolCreateFlags flags = 0);
        VkCommandBufferAllocateInfo CmdBufferAllocInfo(VkCommandPool pool, u32 count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        std::shared_ptr<Window> m_Window;
        i32 m_FrameNumber;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_ChoosenGPU;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VkSwapchainKHR m_Swapchain;
        VkFormat m_SwapchainImageFormat;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkQueue m_GraphicsQueue;
        u32 m_GraphicsQueueFamily;
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_MainCommandBuffer;

        VkRenderPass m_RenderPass;
        std::vector<VkFramebuffer> m_Framebuffers;

        VkSemaphore m_PresentSemaphore;
        VkSemaphore m_RenderSemaphore;
        VkFence m_RenderFence;

        VkPipelineLayout m_TrianglePipelineLayout;
        VkPipeline m_TrianglePipeline;
    };
}
