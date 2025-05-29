#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
class VulkanSurface
{
public:
	VulkanSurface(vk::Instance& instance, GLFWwindow* window);
	~VulkanSurface();
	vk::SurfaceKHR m_surface;
private:
	//internal reference
	vk::Instance& m_instance;
};
