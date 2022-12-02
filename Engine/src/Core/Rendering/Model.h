#pragma once

#include "Core/Rendering/Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace nk {
    class Model {
    public:
        struct Vertex {
            glm::vec2 Position;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        Model(std::shared_ptr<lve::Device> device, const std::vector<Vertex>& vertices);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);

        std::shared_ptr<lve::Device> m_Device;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        u32 m_VertexCount;
    };
} // namespace nk
