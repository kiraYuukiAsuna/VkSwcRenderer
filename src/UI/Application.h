#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vk_mem_alloc.h>

#include <iostream>

class Application {
public:

    void run() {
        initializeWindow();
        initializeVulkan();
        startMainLoop();
        cleanup();
    }

private:
    void initializeWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_GLFWwindow = glfwCreateWindow(m_WindowWidth, 800, "Vulkan Window", nullptr, nullptr);

    }

    void initializeVulkan() {
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Swc Renderer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;
        appInfo.pNext = nullptr;

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;


    }

    void startMainLoop() {
        while (!glfwWindowShouldClose(m_GLFWwindow)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(m_GLFWwindow);

        glfwTerminate();
    }

    int m_WindowWidth{1200};
    int m_WindowHeight{800};
    GLFWwindow *m_GLFWwindow;

    VkInstance m_VkInstance;
};
