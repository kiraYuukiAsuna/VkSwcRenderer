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

    void createIndexBuffer();

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                      vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);

    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    void cleanupCommandBuffer();

    void cleanup();

    vk::CommandPool m_CommandPool;
    std::vector<vk::CommandBuffer> m_CommandBuffers;

    vk::Buffer m_VertexBuffer;
    vk::DeviceMemory m_VertexBufferMemory;

    vk::Buffer m_IndexBuffer;
    vk::DeviceMemory m_IndexBufferMemory;

    std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}

    };

    std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

private:
    Application *m_Application;
};
