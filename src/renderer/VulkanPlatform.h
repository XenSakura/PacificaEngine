#pragma once
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include <vector>

#include "..\core\Window.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"


#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <deque>
#include <functional>
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};

struct FrameData
{
	VkCommandPool commandPool_;
	VkCommandBuffer mainCommandBuffer_;
	VkSemaphore swapchainSemaphore_, renderSemaphore_;
	VkFence renderFence_;
	DeletionQueue deletionQueue_;
};

struct AllocatedImage
{
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkExtent3D imageExtent;
	VkFormat imageFormat;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanPlatform
{
	VulkanPlatform();
	~VulkanPlatform();

	VkInstance instance_;
	VkDebugUtilsMessengerEXT debug_messenger_;
	VkPhysicalDevice chosenGPU_;
	VkDevice device_;
	VkSurfaceKHR surface_;
	VkSwapchainKHR swapchain_;
	VkFormat swapchainImageFormat_;
	vector<VkImage> swapchainImages_;
	vector<VkImageView> swapchainImageViews_;
	VkExtent2D swapchainExtent_;
	VkQueue graphicsQueue_, presentQueue_;
	uint32_t graphicsQueueFamily_, presentQueueFamily_;

	DeletionQueue mainDeletionQueue_;

	VmaAllocator _allocator;

	AllocatedImage drawImage_;
	VkExtent2D drawExtent_;

	VulkanInstance instanceBuilder;
	VulkanSurface surfaceBuilder;
	VulkanDevice deviceBuilder;
	VulkanSwapchain swapchainBuilder;

	void flush_image(VkCommandBuffer cmd);


public:
	void init_vulkan();
	void cleanup_vulkan();

	//we might have to do this across multiple threads
	void init_commands();
	void cleanup_commands();

	void init_sync_structures();
	void cleanup_sync_structures();
	
	void Draw();

	FrameData frames_[FRAME_OVERLAP];
	unsigned frameNumber_;
	FrameData& getCurrentFrame() { return frames_[frameNumber_ % FRAME_OVERLAP]; }
};

