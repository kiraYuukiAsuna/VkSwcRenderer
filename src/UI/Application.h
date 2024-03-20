#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vk_mem_alloc.h>

#include "Util/Logger/Log.h"

class Application {
public:

    Application();

    ~Application();

    Application(Application &) = delete;

    Application(Application &&) = delete;

    Application &operator=(Application &) = delete;

    Application &operator=(Application &&) = delete;

    void run();

private:

    void initializeLogger();

    void initializeWindow();

    void initializeVulkan();

    void startMainLoop();

    void cleanup();

    void createVulkanInstance();

    bool checkValidationLayerSupport();

    std::vector<const char *> getRequiredExtensions();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

    void setupDebugCallback();

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pCallback);

    void DestoryDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback,
                                       const VkAllocationCallbacks *pAllocator);

    int m_WindowWidth{1200};
    int m_WindowHeight{800};
    GLFWwindow *m_GLFWwindow;

    VkInstance m_VkInstance;

    VkDebugUtilsMessengerEXT m_DebugUtilsCallback;

    const std::vector<const char *> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool m_EnableValidationLayers = false;
#else
    const bool m_EnableValidationLayers = true;
#endif

};
