#pragma once
#include "vulkan//vulkan.h"
#include <optional>
#include <vector>

namespace VulkanPlatform
{
	//Max frames at once
	const int MAX_FRAMES_IN_FLIGHT = 2;
	//current frame
	uint32_t currentFrame = 0;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	//for debugging purposes
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	//device esxtensions
	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct DeviceContext
	{
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		VkSurfaceKHR surface;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkInstance instance;
	}vkDeviceContext;

	//current vulkan instance
	class VulkanInstance
	{
	public:
		VkInstance instance;
		VulkanInstance();
		~VulkanInstance();
	}vkInstance;

	class VulkanDebugMessenger
	{
	public:
		VkDebugUtilsMessengerEXT debugMessenger;
		VulkanDebugMessenger();
		~VulkanDebugMessenger();
	}vkDebugMessenger;

	class VulkanPhysicalDevice
	{
	public:
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();
	}vkPhysicalDevice;

	class VulkanDevice
	{
	public:
		VkDevice device;
		VulkanDevice();
		~VulkanDevice();
	}vkDevice;

	//queue families for submitting commands into
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional <uint32_t> presentFamily;
		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	//render commands to graphics queue
	VkQueue graphicsQueue;
	//displays images on screen and presents them to the swapchain
	VkQueue presentQueue;

	//window surface stuff-- connects between vulkan API and the window surface like SDL/glfw
	class VulkanSurface
	{
	public:
		VkSurfaceKHR surface;
		VulkanSurface();
		~VulkanSurface();
	}vkSurface;
	

	//swapchain is used for presenting images to the screen
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	VkSwapchainKHR swapChain;
	//the images that the swapChain swaps between
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	//6. Used for displaying images within the swapchain
	std::vector<VkImageView> swapChainImageViews;

	void SetupDeviceContext();
}
