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

private:
    Application *m_Application;

    VkInstance m_VkInstance;

    vk::PhysicalDevice m_PhysicalDevice;

    vk::Device m_Device;

    vk::Queue m_GraphicsQueue;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;

        bool isComplete() {
            return graphicsFamily >= 0;
        }
    };

    bool isDeviceSuitable(VkPhysicalDevice device);

    void rateDeviceSuitability(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

};
