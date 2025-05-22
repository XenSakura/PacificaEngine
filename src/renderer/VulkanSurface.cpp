#include "VulkanSurface.h"
#include <stdexcept>
#include "..\core\Window.h"
VkSurfaceKHR VulkanSurface::createSurface(VkInstance instance)
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, Window::getInstance().get(), nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}