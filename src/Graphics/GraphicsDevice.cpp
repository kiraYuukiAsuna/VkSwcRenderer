#include "GraphicsDevice.h"
#include "UI/Application.h"

GraphicsDevice::GraphicsDevice(Application *application) : m_Application(application) {

}

void GraphicsDevice::setVkInstance(VkInstance vkInstance) {
    m_VkInstance = vkInstance;
}

void GraphicsDevice::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

    for (const auto &device: devices) {
        if (isDeviceSuitable(device)) {
            m_PhysicalDevice = device;
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

bool GraphicsDevice::isDeviceSuitable(VkPhysicalDevice device) {
//    VkPhysicalDeviceProperties deviceProperties;
//    VkPhysicalDeviceFeatures deviceFeatures;
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//
//    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
//           deviceFeatures.geometryShader;
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

void GraphicsDevice::rateDeviceSuitability(VkPhysicalDevice device) {
    int score = 0;

}

GraphicsDevice::QueueFamilyIndices GraphicsDevice::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily: queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }


    return indices;
}

void GraphicsDevice::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

    vk::DeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfo.pNext = nullptr;

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo createInfo{};
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (m_Application->m_EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_Application->m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_Application->m_ValidationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    m_Device = m_PhysicalDevice.createDevice(createInfo);

    m_GraphicsQueue = m_Device.getQueue(indices.graphicsFamily, 0);
}

GraphicsDevice::~GraphicsDevice() {
    m_Device.destroy();
}


