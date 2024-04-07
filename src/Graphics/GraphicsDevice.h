#pragma once

#include <vulkan/vulkan.hpp>
#include <Util/Logger/Log.h>

class Application;

class GraphicsDevice {
public:
    GraphicsDevice(Application *application);

    ~GraphicsDevice();

    void setVkInstance(VkInstance vkInstance);

    void pickPhysicalDevice();

    void createLogicalDevice();

    void cleanup();

    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::Queue m_GraphicsQueue;
    vk::Queue m_PresentQueue;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() {
            return graphicsFamily >= 0 && presentFamily >= 0;
        }
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    uint32_t findMemoryType(uint32_t i, vk::Flags<vk::MemoryPropertyFlagBits> flags);

private:
    Application *m_Application;

    VkInstance m_VkInstance;

    bool isDeviceSuitable(VkPhysicalDevice device);

    void rateDeviceSuitability(VkPhysicalDevice device);

};
