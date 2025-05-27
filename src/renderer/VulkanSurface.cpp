#include "VulkanSurface.h"
#include "GLFW/glfw3.h"
#include "..\core\Window.h"
namespace Vulkan
{
	bool createVkSurface(VkInstance& instance, VkSurfaceKHR& surface)
	{
		if (glfwCreateWindowSurface(instance, Window::getInstance().get(), nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("ERROR: Failed to create window surface!");
		}
		return true;
	}

	bool destroyVkSurface(VkInstance& instance, VkSurfaceKHR& surface)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
		return true;
	}
}
