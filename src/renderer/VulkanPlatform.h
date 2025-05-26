#pragma once
#include "vulkan/vulkan.h"
#include "IAPIPlatform.h"
//this is the Vulkan backend
class VulkanPlatform : public IAPIPlatform
{
	//only need one of each
	VkInstance m_instance;

	VkPhysicalDevice m_physicalDevice;

	VkDevice m_device;

	VkQueue m_queue;

	VkCommandPool m_cmdpool;

	


};