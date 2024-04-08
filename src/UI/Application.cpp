#include "Application.h"


Application::Application() : m_GraphicsDevice(this), m_WindowSurface(this), m_SwapChain(this),
                             m_GraphicsPipeline(this), m_CommandBuffer(this) {

}

Application::~Application() {
    for (int i = 0; i < MaxFramesInFlight; i++) {
        m_GraphicsDevice.m_Device.destroySemaphore(m_ImageAvailableSemaphores[i], nullptr);
        m_GraphicsDevice.m_Device.destroySemaphore(m_RenderFinishedSemaphores[i], nullptr);
        m_GraphicsDevice.m_Device.destroyFence(m_InFlightFences[i], nullptr);
    }
    m_CommandBuffer.cleanup();
    m_GraphicsPipeline.cleanup();
    m_SwapChain.cleanup();
    m_GraphicsDevice.cleanup();

    if (m_EnableValidationLayers) {
        DestoryDebugUtilsMessengerEXT(m_VkInstance, m_DebugUtilsCallback, nullptr);
    }

    m_WindowSurface.cleanup();

    vkDestroyInstance(m_VkInstance, nullptr);

    glfwDestroyWindow(m_GLFWwindow);

    glfwTerminate();
}

void Application::run() {
    initializeLogger();
    initializeWindow();
    initializeVulkan();
    startMainLoop();
}

void Application::initializeLogger() {
    Seele::Log::Init();
}

void Application::initializeWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWwindow = glfwCreateWindow(m_WindowWidth, 800, "VkSwcRenderer", nullptr, nullptr);
    glfwSetWindowUserPointer(m_GLFWwindow, this);
    glfwSetFramebufferSizeCallback(m_GLFWwindow, framebufferResizeCallback);
}

void Application::initializeVulkan() {
    createVulkanInstance();
    setupDebugCallback();

    m_WindowSurface.setVkInstance(m_VkInstance);
    m_WindowSurface.createSurface(m_GLFWwindow);

    m_GraphicsDevice.setVkInstance(m_VkInstance);
    m_GraphicsDevice.pickPhysicalDevice();
    m_GraphicsDevice.createLogicalDevice();

    m_SwapChain.createSwapChain();

    m_GraphicsPipeline.createRenderPass();
    m_GraphicsPipeline.create();

    m_SwapChain.createImageViews();
    m_SwapChain.createFramebuffers();

    m_CommandBuffer.CreateCommandPool();
    m_CommandBuffer.createCommandBuffers();
    m_CommandBuffer.createVertexBuffer();
    m_CommandBuffer.createIndexBuffer();

    createSyncObjects();
}

void Application::startMainLoop() {
    while (!glfwWindowShouldClose(m_GLFWwindow)) {
        glfwPollEvents();
        drawFrame();
    }

    m_GraphicsDevice.m_Device.waitIdle();
}

void Application::createVulkanInstance() {
    if (m_EnableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Swc Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.pNext = nullptr;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (m_EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    createInfo.flags = 0;
    createInfo.pNext = nullptr;

    auto a = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
    if (a != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance");
    }
}

bool Application::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName: m_ValidationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                SEELE_INFO_TAG(__func__, "Found {}", layerName);
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            SEELE_ERROR_TAG(__func__, "Cannot Found {}", layerName);
            return false;
        }
    }

    return true;
}

std::vector<const char *> Application::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_EnableValidationLayers) {
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    uint32_t availableExtensionsCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

    SEELE_INFO_TAG(__func__, "{} Available Extensions Supported:", availableExtensions.size());
    for (const auto &extension: availableExtensions) {
        SEELE_INFO_TAG(__func__, "{}", extension.extensionName);
    }
    for (auto &requiredExtension: requiredExtensions) {
        bool layerFound = false;

        for (auto &availableExtension: availableExtensions) {
            if (strcmp(availableExtension.extensionName, requiredExtension) == 0) {
                SEELE_INFO_TAG(__func__, "Found {}", availableExtension.extensionName);
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            std::string errorMessage =
                    std::string("Cannot Found Extension ") + requiredExtension;
            throw std::runtime_error(errorMessage);
        }
    }

    return requiredExtensions;
}

void Application::setupDebugCallback() {
    if (!m_EnableValidationLayers) { return; }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;

    if (CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_DebugUtilsCallback) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug callback!");
    }
}

VkResult
Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pCallback) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                           "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Application::DestoryDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback,
                                                const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

VkBool32 Application::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        SEELE_WARN_TAG(__func__, "Validation Layer: {}", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

void Application::drawFrame() {
    m_GraphicsDevice.m_Device.waitForFences(1, &m_InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto result = m_GraphicsDevice.m_Device.acquireNextImageKHR(m_SwapChain.m_SwapChain, UINT64_MAX,
                                                                m_ImageAvailableSemaphores[currentFrame], nullptr,
                                                                &imageIndex);
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    m_GraphicsDevice.m_Device.resetFences(1, &m_InFlightFences[currentFrame]);

    m_CommandBuffer.recordCommandBuffers(imageIndex);

    vk::SubmitInfo submitInfo;
    vk::Semaphore waitSemaphores[] = {m_ImageAvailableSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::Semaphore signalSemaphores[] = {m_RenderFinishedSemaphores[currentFrame]};

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer.m_CommandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (m_GraphicsDevice.m_GraphicsQueue.submit(1, &submitInfo, m_InFlightFences[currentFrame]) !=
        vk::Result::eSuccess) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_SwapChain.m_SwapChain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = m_GraphicsDevice.m_PresentQueue.presentKHR(&presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_FramebufferResized) {
        m_FramebufferResized = false;
        recreateSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image");
    }

    currentFrame = (currentFrame + 1) % MaxFramesInFlight;
}

void Application::createSyncObjects() {
    m_ImageAvailableSemaphores.resize(MaxFramesInFlight);
    m_RenderFinishedSemaphores.resize(MaxFramesInFlight);
    m_InFlightFences.resize(MaxFramesInFlight);


    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    for (int i = 0; i < MaxFramesInFlight; i++) {
        if (m_GraphicsDevice.m_Device.createSemaphore(&semaphoreInfo, nullptr,
                                                      &m_ImageAvailableSemaphores[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create semaphores");
        }
        if (m_GraphicsDevice.m_Device.createSemaphore(&semaphoreInfo, nullptr,
                                                      &m_RenderFinishedSemaphores[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create semaphores");
        }
        if (m_GraphicsDevice.m_Device.createFence(&fenceInfo, nullptr, &m_InFlightFences[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create fences");
        }
    }

}

void Application::recreateSwapChain() {
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_GLFWwindow, &width, &height);
        glfwWaitEvents();
    }

    m_GraphicsDevice.m_Device.waitIdle();

    m_SwapChain.cleanup();
    m_CommandBuffer.cleanupCommandBuffer();
    m_GraphicsPipeline.cleanup();

    m_SwapChain.createSwapChain();
    m_SwapChain.createImageViews();
    m_GraphicsPipeline.createRenderPass();
    m_GraphicsPipeline.create();
    m_SwapChain.createFramebuffers();
    m_CommandBuffer.createCommandBuffers();
}

void Application::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    app->m_FramebufferResized = true;
}
