#include "VulkanSurface.h"


VulkanSurface::VulkanSurface(vk::Instance& instance, GLFWwindow* window)
	:m_instance(instance)
{
	VkSurfaceKHR temp;
	VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &temp);

	m_surface = vk::SurfaceKHR(temp);
	
}


VulkanSurface::~VulkanSurface()
{
	if (m_surface)
		m_instance.destroySurfaceKHR(m_surface);
}
