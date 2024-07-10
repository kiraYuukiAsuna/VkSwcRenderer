#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <iostream>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Graphics/GraphicsDevice.h"
#include "Graphics/WindowSurface.h"
#include "Graphics/SwapChain.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/CommandBuffer.h"

// 全局变量来存储变换信息
extern float scale;
extern float rotation;
extern float positionX;
extern float positionY;

class Application {
public:
    ~Application();

    Application(Application &) = delete;

    Application(Application &&) = delete;

    Application &operator=(Application &) = delete;

    Application &operator=(Application &&) = delete;

    static Application &getInstance() {
        static Application instance;
        return instance;
    }

    void run();

    void drawFrame();

    void createSyncObjects();

    void recreateSwapChain();

    void loadSwc(const std::string& filePath);

    GraphicsDevice m_GraphicsDevice;
    WindowSurface m_WindowSurface;
    SwapChain m_SwapChain;
    GraphicsPipeline m_GraphicsPipeline;
    CommandBuffer m_CommandBuffer;
    GLFWwindow *m_GLFWwindow;

    std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
    std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
    std::vector<vk::Fence> m_InFlightFences;

    static constexpr int MaxFramesInFlight = 2;

    bool m_FramebufferResized = false;

    int currentFrame = 0;
#ifdef NDEBUG
    const bool m_EnableValidationLayers = false;
#else
    const bool m_EnableValidationLayers = true;
#endif

    const std::vector<const char *> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

private:
    Application();

    void initializeWindow();

    void initializeVulkan();

    void startMainLoop();

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

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);



    // 键盘事件处理
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
                case GLFW_KEY_UP:
                    positionY += 0.1f;
                break;
                case GLFW_KEY_DOWN:
                    positionY -= 0.1f;
                break;
                case GLFW_KEY_LEFT:
                    positionX -= 0.1f;
                break;
                case GLFW_KEY_RIGHT:
                    positionX += 0.1f;
                break;
                case GLFW_KEY_A:
                    rotation += 5.0f;
                break;
                case GLFW_KEY_D:
                    rotation -= 5.0f;
                break;
                case GLFW_KEY_W:
                    scale *= 1.1f;
                break;
                case GLFW_KEY_S:
                    scale /= 1.1f;
                break;
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            }
        }
    }

    // 鼠标事件处理
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            std::cout << "Mouse Position at (" << xpos << " : " << ypos << ")\n";
        }
    }

    int m_WindowWidth{1200};
    int m_WindowHeight{800};

    VkInstance m_VkInstance;

    VkDebugUtilsMessengerEXT m_DebugUtilsCallback;


};
