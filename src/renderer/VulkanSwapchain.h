#pragma once
#include <vector>
#include <vulkan/vulkan.h>
using namespace std;

struct SwapchainContext
{
	VkSwapchainKHR swapchain;
	VkFormat swapchainImageFormat;

	vector<VkImage> swapchainImages;
	vector<VkImageView> swapchainImageViews;
	VkExtent2D swapchainExtent;
	
};

class VulkanSwapchain
{

public:
	SwapchainContext createSwapchain(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice device);
};
