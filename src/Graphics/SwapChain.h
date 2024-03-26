#pragma once

#include <vulkan/vulkan.hpp>

class Application;

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class SwapChain {
public:
    SwapChain(Application* application);
    ~SwapChain();

    bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    void createSwapChain();

    void createImageViews();

    void createFramebuffers();

    void cleanup();

    const std::vector<const char *> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::SwapchainKHR m_SwapChain;

    std::vector<vk::Image> m_SwapChainImages;
    std::vector<vk::ImageView> m_SwapChainImageViews;
    vk::Format m_SwapChainImageFormat;
    vk::Extent2D m_SwapChainExtent;
    std::vector<vk::Framebuffer> m_SwapChainFramebuffers;


private:
    Application *m_Application;
};

