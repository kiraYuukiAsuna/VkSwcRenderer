#include "GraphicsDevice.h"
#include "UI/Application.h"
#include <set>

GraphicsDevice::GraphicsDevice(Application *application) : m_Application(application) {

}

void GraphicsDevice::setVkInstance(VkInstance vkInstance) {
    m_VkInstance = vkInstance;
}

GraphicsDevice::~GraphicsDevice() {

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
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = m_Application->m_SwapChain.checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = m_Application->m_SwapChain.querySwapChainSupport(device,
                                                                                                    m_Application->m_WindowSurface.m_Surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

//    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    return deviceFeatures.geometryShader && indices.isComplete() && extensionsSupported && swapChainAdequate;
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

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Application->m_WindowSurface.m_Surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
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

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    float queuePriority = 1.0f;

    for (int queueFamily: uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo createInfo{};
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_Application->m_SwapChain.m_DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_Application->m_SwapChain.m_DeviceExtensions.data();

    if (m_Application->m_EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_Application->m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_Application->m_ValidationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    m_Device = m_PhysicalDevice.createDevice(createInfo);

    m_GraphicsQueue = m_Device.getQueue(indices.graphicsFamily, 0);
    m_PresentQueue = m_Device.getQueue(indices.presentFamily, 0);
}

void GraphicsDevice::cleanup() {
    m_Device.destroy();
}



