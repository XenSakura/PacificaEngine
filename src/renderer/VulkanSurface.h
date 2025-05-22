#pragma once

#include <vulkan/vulkan.h>

class VulkanSurface
{
public:
	VkSurfaceKHR createSurface(VkInstance instance);
};
