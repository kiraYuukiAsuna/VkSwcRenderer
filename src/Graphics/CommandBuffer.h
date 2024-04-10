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

    void recordCommandBuffers(uint32_t imageIndex, std::vector<vk::DescriptorSet>&descriptorSets);

    void cleanupCommandBuffer();

    void cleanup();


private:
    Application *m_Application;
};
