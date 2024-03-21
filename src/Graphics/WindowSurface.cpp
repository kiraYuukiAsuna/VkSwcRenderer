#include "WindowSurface.h"

WindowSurface::WindowSurface(Application *application) : m_Application(application) {

}

WindowSurface::~WindowSurface() {
}

void WindowSurface::setVkInstance(vk::Instance vkInstance) {
    m_VkInstance = vkInstance;
}

void WindowSurface::createSurface(GLFWwindow *window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(m_VkInstance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
    m_Surface = surface;
}

void WindowSurface::cleanup() {
    vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);

}




