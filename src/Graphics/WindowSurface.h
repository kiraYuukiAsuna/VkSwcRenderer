#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

class Application;

class WindowSurface {
public:
    WindowSurface(Application* application);
    ~WindowSurface();

    void setVkInstance(vk::Instance vkInstance);

    void createSurface(GLFWwindow *window);

    void cleanup();

    vk::SurfaceKHR m_Surface;

private:
    vk::Instance m_VkInstance;
    Application *m_Application;
};

