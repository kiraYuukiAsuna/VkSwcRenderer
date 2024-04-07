#pragma once

#include <vulkan/vulkan.hpp>
#include "Object/Type.hpp"

class Application;

class CommandBuffer {
public:
    CommandBuffer(Application *application);

    ~CommandBuffer();

    void CreateCommandPool();

    void createCommandBuffers();

    void recordCommandBuffers(uint32_t imageIndex);

    void createVertexBuffer();

    void cleanupCommandBuffer();

    void cleanup();

    vk::CommandPool m_CommandPool;
    std::vector<vk::CommandBuffer> m_CommandBuffers;

    vk::Buffer m_VertexBuffer;
    vk::DeviceMemory m_VertexBufferMemory;

    std::vector<Vertex> vertices = {
            {{0.0f,  -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
    };

private:
    Application *m_Application;
};
