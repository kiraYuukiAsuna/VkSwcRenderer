#pragma once

#include <vulkan/vulkan.hpp>

class Application;

class CommandBuffer {
public:
    CommandBuffer(Application *application);

    ~CommandBuffer();

    void CreateCommandPool();

    void createCommandBuffers();

    void recordCommandBuffers(uint32_t imageIndex);

    void cleanup();

    vk::CommandPool m_CommandPool;
    std::vector<vk::CommandBuffer> m_CommandBuffers;

private:
    Application *m_Application;
};
