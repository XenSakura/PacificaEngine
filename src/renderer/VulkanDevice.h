#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include "VulkanPlatform.h"

struct DeviceContext
{
	VkDevice device;
	VkQueue graphicsQueue, presentQueue;
	uint32_t graphicsQueueFamily, presentQueueFamily;
};
class VulkanDevice
{
private:
public:
	VkPhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
	DeviceContext createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};
