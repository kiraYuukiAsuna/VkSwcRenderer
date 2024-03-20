#include "Application.h"


Application::Application() {

}

Application::~Application() {

}

void Application::run() {
    initializeLogger();
    initializeWindow();
    initializeVulkan();
    startMainLoop();
    cleanup();
}

void Application::initializeLogger() {
    Seele::Log::Init();
}

void Application::initializeWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWwindow = glfwCreateWindow(m_WindowWidth, 800, "VkSwcRenderer", nullptr, nullptr);
}

void Application::initializeVulkan() {
    createVulkanInstance();
    setupDebugCallback();
}

void Application::startMainLoop() {
    while (!glfwWindowShouldClose(m_GLFWwindow)) {
        glfwPollEvents();
    }
}

void Application::cleanup() {
    if (m_EnableValidationLayers) {
        DestoryDebugUtilsMessengerEXT(m_VkInstance, m_DebugUtilsCallback, nullptr);
    }

    vkDestroyInstance(m_VkInstance, nullptr);

    glfwDestroyWindow(m_GLFWwindow);

    glfwTerminate();
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