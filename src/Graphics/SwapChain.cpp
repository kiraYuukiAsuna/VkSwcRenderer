#include "SwapChain.h"
#include <set>
#include "UI/Application.h"

SwapChain::SwapChain(Application *application) : m_Application(application) {

}

SwapChain::~SwapChain() {
}

bool SwapChain::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

    for (const auto &extension: availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    SwapChainSupportDetails details;

    if (device.getSurfaceCapabilitiesKHR(surface, &details.capabilities) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to get surface capabilities!");
    }

    uint32_t formatCount;

    details.formats  = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
        return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
    }

    for (const auto &availableFormat: availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto &availablePresentMode: availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(m_Application->m_GLFWwindow, &width, &height);

        vk::Extent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_Application->m_GraphicsDevice.m_PhysicalDevice,
                                                                     m_Application->m_WindowSurface.m_Surface);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.surface = m_Application->m_WindowSurface.m_Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    GraphicsDevice::QueueFamilyIndices indices = m_Application->m_GraphicsDevice.findQueueFamilies(
            m_Application->m_GraphicsDevice.m_PhysicalDevice);
    uint32_t queueFamilyIndices[] = {(uint32_t) indices.graphicsFamily, (uint32_t) indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;

    if (m_Application->m_GraphicsDevice.m_Device.createSwapchainKHR(&createInfo, nullptr, &m_SwapChain) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create swap chain!");
    }

    m_SwapChainImages = m_Application->m_GraphicsDevice.m_Device.getSwapchainImagesKHR(m_SwapChain);
    m_SwapChainImageFormat = surfaceFormat.format;
    m_SwapChainExtent = extent;
}

void SwapChain::cleanup() {
    vkDestroySwapchainKHR(m_Application->m_GraphicsDevice.m_Device, m_SwapChain, nullptr);
}
