#pragma once


#include "vulkan/vulkan.h"
#include <iostream>
#include <vector>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


struct RenderContext
{
	VkDevice m_device;

	VkQueue m_presentQueue, m_graphicsQueue;

	VkSwapchainKHR m_swapchain;

	// images are multidimensional arrays of data for attachments
	//textures etc
	std::vector<VkImage> m_swapchainImages;

	//image formats-- describes metadata of colors
	VkFormat m_swapchainImageFormat;

	//also metadata-- describes height and width of the swapchain/image
	VkExtent2D m_swapchainExtent;

	//the metadata for the various images for the pipeline shaders to access
	std::vector<VkImageView> m_swapchainImageViews;

	VkCommandPool m_cmdpool;
};