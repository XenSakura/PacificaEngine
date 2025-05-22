#pragma once
#include "vulkan/vulkan.h"



class VulkanInstance
{
public:
	VkInstance createInstance();
	VkDebugUtilsMessengerEXT setupDebugMessenger(VkInstance instance);
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif