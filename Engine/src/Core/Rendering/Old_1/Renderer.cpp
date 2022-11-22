#include "nkpch.h"

#include "Renderer.h"

#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Core/Application.h"
#include "Core/Rendering/PipelineBuilder.h"

namespace nk {
    Renderer::Renderer(const std::string& application_name, std::shared_ptr<Window> window)
        : m_Window(std::move(window)) {
        InitVulkan(application_name);
        InitSwapchain();
        InitCommands();
        InitDefaultRenderpass();
        InitFramebuffers();
        InitSyncStructures();
        InitPipeline();
    }

    Renderer::~Renderer() {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        
        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

        for (int i = 0; i < m_SwapchainImageViews.size(); ++i) {
            vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
            vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
        }

        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void Renderer::Draw() {
        VK_CHECK(vkWaitForFences(m_Device, 1, &m_RenderFence, true, 1000000000));
        VK_CHECK(vkResetFences(m_Device, 1, &m_RenderFence));

        u32 swapchain_image_index;
        VK_CHECK(vkAcquireNextImageKHR(m_Device, m_Swapchain, 1000000000, m_PresentSemaphore, nullptr, &swapchain_image_index));

        VK_CHECK(vkResetCommandBuffer(m_MainCommandBuffer, 0));

        VkCommandBuffer cmd = m_MainCommandBuffer;
        VkCommandBufferBeginInfo cmd_begin_info = {};
        cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_begin_info.pNext = nullptr;
        cmd_begin_info.pInheritanceInfo = nullptr;
        cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

        // FIX: Test clear color
        VkClearValue clear_value;
        f32 flash = glm::abs(glm::sin(m_FrameNumber / 120.0f));
        clear_value.color = {{ 0.0f, 0.0f, flash, 1.0f }};

        VkRenderPassBeginInfo renderpass_begin_info = {};
        renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpass_begin_info.pNext = nullptr;
        renderpass_begin_info.renderPass = m_RenderPass;
        renderpass_begin_info.renderArea.offset.x = 0;
        renderpass_begin_info.renderArea.offset.y = 0;
        renderpass_begin_info.renderArea.extent = VkExtent2D{m_Window->GetWidth(), m_Window->GetHeight()};
        renderpass_begin_info.framebuffer = m_Framebuffers[swapchain_image_index];
        renderpass_begin_info.clearValueCount = 1;
        renderpass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(cmd, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);
        vkCmdDraw(cmd, 3, 1, 0, 0);

        vkCmdEndRenderPass(cmd);

        VK_CHECK(vkEndCommandBuffer(cmd));

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.pWaitDstStageMask = &wait_stage;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &m_PresentSemaphore;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &m_RenderSemaphore;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;
        VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submit_info, m_RenderFence));

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;
        present_info.pSwapchains = &m_Swapchain;
        present_info.swapchainCount = 1;
        present_info.pWaitSemaphores = &m_RenderSemaphore;
        present_info.waitSemaphoreCount = 1;
        present_info.pImageIndices = &swapchain_image_index;
        VK_CHECK(vkQueuePresentKHR(m_GraphicsQueue, &present_info));

        ++m_FrameNumber;
    }

    bool Renderer::LoadShaderModule(const char* file_path, VkShaderModule* out_shader_module) {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            return false;

        size_t file_size = (size_t)file.tellg();
        std::vector<u32> buffer(file_size / sizeof(u32));

        file.seekg(0);
        file.read((char*)buffer.data(), file_size);
        file.close();

        VkShaderModuleCreateInfo shader_module_info = {};
        shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_info.pNext = nullptr;
        shader_module_info.codeSize = buffer.size() * sizeof(u32);
        shader_module_info.pCode = buffer.data();

        VkShaderModule shader_module;
        if (vkCreateShaderModule(m_Device, &shader_module_info, nullptr, &shader_module) != VK_SUCCESS) {
            return false;
        }

        *out_shader_module = shader_module;
        return true;
    }

    void Renderer::InitVulkan(const std::string& application_name) {
        vkb::InstanceBuilder instance_builder;
        auto instance_returner = instance_builder.set_app_name(application_name.c_str())
            .request_validation_layers(true)
            .require_api_version(1, 1, 0)
            .use_default_debug_messenger()
            .build();
        vkb::Instance vkb_instance = instance_returner.value();
        m_Instance = vkb_instance.instance;
        m_DebugMessenger = vkb_instance.debug_messenger;

        auto window = m_Window->GetNativeWindow();
        const auto success = glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface);
        CoreAssertMsg(success == VK_SUCCESS, "Failed to create Vulkan Surface at Renderer!");
        
        vkb::PhysicalDeviceSelector selector(vkb_instance);
        vkb::PhysicalDevice vkb_physical_device = selector.set_minimum_version(1, 1)
            .set_surface(m_Surface)
            .select()
            .value();

        vkb::DeviceBuilder device_builder(vkb_physical_device);
        vkb::Device vkb_device = device_builder.build().value();

        m_Device = vkb_device.device;
        m_ChoosenGPU = vkb_physical_device.physical_device;

        m_GraphicsQueue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueueFamily = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    }

    void Renderer::InitSwapchain() {
        vkb::SwapchainBuilder swapchain_builder(m_ChoosenGPU, m_Device, m_Surface);
        vkb::Swapchain vkb_swapchain = swapchain_builder.use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(m_Window->GetWidth(), m_Window->GetHeight())
            .build()
            .value();
        m_Swapchain = vkb_swapchain.swapchain;
        m_SwapchainImages = vkb_swapchain.get_images().value();
        m_SwapchainImageViews = vkb_swapchain.get_image_views().value();
        m_SwapchainImageFormat = vkb_swapchain.image_format;
    }

    void Renderer::InitCommands() {
        VkCommandPoolCreateInfo command_pool_info = CommandPoolCreateInfo(m_GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        VK_CHECK(vkCreateCommandPool(m_Device, &command_pool_info, nullptr, &m_CommandPool));

        VkCommandBufferAllocateInfo cmd_alloc_info = CmdBufferAllocInfo(m_CommandPool);
        VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmd_alloc_info, &m_MainCommandBuffer));
    }

    void Renderer::InitDefaultRenderpass() {
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = m_SwapchainImageFormat;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        VK_CHECK(vkCreateRenderPass(m_Device, &render_pass_info, nullptr, &m_RenderPass));
    }

    void Renderer::InitFramebuffers() {
        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.pNext = nullptr;
        framebuffer_info.renderPass = m_RenderPass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.width = m_Window->GetWidth();
        framebuffer_info.height = m_Window->GetHeight();
        framebuffer_info.layers = 1;

        const u32 swapchain_image_count = m_SwapchainImages.size();
        m_Framebuffers = std::vector<VkFramebuffer>(swapchain_image_count);
        for (int i = 0; i < swapchain_image_count; ++i) {
            framebuffer_info.pAttachments = &m_SwapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(m_Device, &framebuffer_info, nullptr, &m_Framebuffers[i]));
        }
    }

    void Renderer::InitSyncStructures() {
        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.pNext = nullptr;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(m_Device, &fence_info, nullptr, &m_RenderFence));

        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_info.pNext = nullptr;
        semaphore_info.flags = 0;
        VK_CHECK(vkCreateSemaphore(m_Device, &semaphore_info, nullptr, &m_PresentSemaphore));
        VK_CHECK(vkCreateSemaphore(m_Device, &semaphore_info, nullptr, &m_RenderSemaphore));
    }

    void Renderer::InitPipeline() {
        VkShaderModule triangle_frag_shader;
        if (!LoadShaderModule("shaders/colored_triangle.frag.spv", &triangle_frag_shader)) {
            CoreWarn("Error when building the triangle fragment shader module.");
        } else {
            CoreDebug("Triangle fragment shader created.");
        }

        VkShaderModule triangle_vert_shader;
        if (!LoadShaderModule("shaders/colored_triangle.vert.spv", &triangle_vert_shader)) {
            CoreWarn("Error when building the triangle vertex shader module.");
        } else {
            CoreDebug("Triangle vertex shader created.");
        }

        PipelineBuilder pipeline_builder;

        VkPipelineLayoutCreateInfo pipeline_layout_info = pipeline_builder.LayoutCreateInfo();
        VK_CHECK(vkCreatePipelineLayout(m_Device, &pipeline_layout_info, nullptr, &m_TrianglePipelineLayout));

        pipeline_builder.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, triangle_vert_shader);
        pipeline_builder.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, triangle_frag_shader);

        pipeline_builder.SetInputAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        pipeline_builder.SetViewport(0.0f, 0.0f, (f32)m_Window->GetWidth(), (f32)m_Window->GetHeight(), 0.0f, 1.0f);
        pipeline_builder.SetScissor({ 0, 0 }, { m_Window->GetWidth(), m_Window->GetHeight() });

        pipeline_builder.SetRasterizationInfo(VK_POLYGON_MODE_FILL);

        pipeline_builder.SetPipelineLayout(m_TrianglePipelineLayout);
        m_TrianglePipeline = pipeline_builder.BuildPipeline(m_Device, m_RenderPass);
    }

    VkCommandPoolCreateInfo Renderer::CommandPoolCreateInfo(u32 queue_family_index, VkCommandPoolCreateFlags flags) {
        VkCommandPoolCreateInfo command_pool_info = {};
        command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_info.pNext = nullptr;
        command_pool_info.queueFamilyIndex = queue_family_index;
        command_pool_info.flags = flags;
        return command_pool_info;
    }

    VkCommandBufferAllocateInfo Renderer::CmdBufferAllocInfo(VkCommandPool pool, u32 count, VkCommandBufferLevel level) {
        VkCommandBufferAllocateInfo cmd_alloc_info = {};
        cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_alloc_info.pNext = nullptr;
        cmd_alloc_info.commandPool = pool;
        cmd_alloc_info.commandBufferCount = count;
        cmd_alloc_info.level = level;
        return cmd_alloc_info;
    }
}
