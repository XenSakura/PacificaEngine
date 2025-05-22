#include "VulkanPlatform.h"
#include <iostream>

VulkanPlatform::VulkanPlatform() {}

VulkanPlatform::~VulkanPlatform() {}

void VulkanPlatform::init_vulkan()
{
	
	instance_ = instanceBuilder.createInstance();
	debug_messenger_ = instanceBuilder.setupDebugMessenger(instance_);
	surface_ = surfaceBuilder.createSurface(instance_);
	chosenGPU_ = deviceBuilder.pickPhysicalDevice(instance_, surface_);
	auto devContext = deviceBuilder.createLogicalDevice(chosenGPU_, surface_);
	device_ = devContext.device;
	auto swapContext = swapchainBuilder.createSwapchain(surface_, chosenGPU_, device_);
	{
		//draw image size will match the window
		VkExtent3D drawImageExtent = {
			Window::getInstance().width,
			Window::getInstance().height,
			1
		};

		//hardcoding the draw format to 32 bit float
		drawImage_.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		drawImage_.imageExtent = drawImageExtent;

		VkImageUsageFlags drawImageUsages{};
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo rimg_info{};
		rimg_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		rimg_info.imageType = VK_IMAGE_TYPE_2D;
		rimg_info.extent.width = drawImageExtent.width;
		rimg_info.extent.height = drawImageExtent.height;
		rimg_info.extent.depth = 1; // 2D image → depth is 1
		rimg_info.mipLevels = 1;
		rimg_info.arrayLayers = 1;
		rimg_info.format = drawImage_.imageFormat;
		rimg_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		rimg_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		rimg_info.usage = drawImageUsages; // e.g., VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
		rimg_info.samples = VK_SAMPLE_COUNT_1_BIT;
		rimg_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
		//for the draw image, we want to allocate it from gpu local memory
		VmaAllocationCreateInfo rimg_allocinfo = {};
		rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		//allocate and create the image
		vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &drawImage_.image, &drawImage_.allocation, nullptr);

		//build a image-view for the draw image to use for rendering
		VkImageViewCreateInfo rview_info{};
		rview_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		rview_info.image = drawImage_.image; // the VkImage you're viewing
		rview_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // most common for screen-space rendering
		rview_info.format = drawImage_.imageFormat;  // must match the image's format

		rview_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // for color attachment
		rview_info.subresourceRange.baseMipLevel = 0;
		rview_info.subresourceRange.levelCount = 1;
		rview_info.subresourceRange.baseArrayLayer = 0;
		rview_info.subresourceRange.layerCount = 1;

		rview_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		rview_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		rview_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		rview_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;


		vkCreateImageView(device_, &rview_info, nullptr, &drawImage_.imageView);

		//add to deletion queues
		mainDeletionQueue_.push_function([=]() {
			vkDestroyImageView(device_, drawImage_.imageView, nullptr);
			vmaDestroyImage(_allocator, drawImage_.image, drawImage_.allocation);
			});
	}
	swapchain_ = swapContext.swapchain;
	swapchainImageFormat_ = swapContext.swapchainImageFormat;
	swapchainImages_ = swapContext.swapchainImages;
	swapchainImageViews_ = swapContext.swapchainImageViews;
	swapchainExtent_ = swapContext.swapchainExtent;
	graphicsQueue_ = devContext.graphicsQueue;
	graphicsQueueFamily_ = devContext.graphicsQueueFamily;
	presentQueue_ = devContext.presentQueue;
	presentQueueFamily_ = devContext.presentQueueFamily;
	
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = chosenGPU_;
	allocatorInfo.device = device_;
	allocatorInfo.instance = instance_;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&allocatorInfo, &_allocator);

	mainDeletionQueue_.push_function([&]()
		{
			vmaDestroyAllocator(_allocator);
		});
}

void VulkanPlatform::cleanup_vulkan()
{
	cleanup_commands();

	cleanup_sync_structures();

	for (int i = 0; i < swapchainImageViews_.size(); ++i)
	{
		vkDestroyImageView(device_, swapchainImageViews_[i], nullptr);
	}

	vkDestroySwapchainKHR(device_, swapchain_, nullptr);

	vkDestroySurfaceKHR(instance_, surface_, nullptr);
	vkDestroyDevice(device_, nullptr);

	if (enableValidationLayers)
	{
		vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
	}
	
	vkDestroyInstance(instance_, nullptr);

}

//refactor for multithreaded command buffer recording later
void VulkanPlatform::init_commands()
{
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = graphicsQueueFamily_;

	for (int i = 0; i < FRAME_OVERLAP; i++) {

		vkCreateCommandPool(device_, &commandPoolInfo, nullptr, &frames_[i].commandPool_);

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.pNext = nullptr;
		cmdAllocInfo.commandPool = frames_[i].commandPool_;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		vkAllocateCommandBuffers(device_, &cmdAllocInfo, &frames_[i].mainCommandBuffer_);
	}
}

void VulkanPlatform::cleanup_commands()
{
	vkDeviceWaitIdle(device_);

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		vkDestroyCommandPool(device_, frames_[i].commandPool_, nullptr);
	}
}

void VulkanPlatform::init_sync_structures()
{
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	for (int i = 0; i < FRAME_OVERLAP; ++i)
	{
		vkCreateFence(device_, &fenceCreateInfo, nullptr, &frames_[i].renderFence_);

		vkCreateSemaphore(device_, &semaphoreCreateInfo, nullptr, &frames_[i].swapchainSemaphore_);
		vkCreateSemaphore(device_, &semaphoreCreateInfo, nullptr, &frames_[i].renderSemaphore_);
	}
}

void VulkanPlatform::cleanup_sync_structures()
{
	for (int i = 0; i < FRAME_OVERLAP; ++i)
	{
		vkDestroyFence(device_, frames_[i].renderFence_, nullptr);
		vkDestroySemaphore(device_, frames_[i].renderSemaphore_, nullptr);
		vkDestroySemaphore(device_, frames_[i].swapchainSemaphore_, nullptr);
		frames_[i].deletionQueue_.flush();
	}
	mainDeletionQueue_.flush();
}

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
	imageBarrier.pNext = nullptr;

	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = currentLayout;
	imageBarrier.newLayout = newLayout;

	VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageSubresourceRange subImage{};
	subImage.aspectMask = aspectMask;
	subImage.baseMipLevel = 0;
	subImage.levelCount = VK_REMAINING_MIP_LEVELS;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	imageBarrier.subresourceRange = subImage;
	imageBarrier.image = image;

	VkDependencyInfo depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.pNext = nullptr;

	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(cmd, &depInfo);
}
void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize)
{
	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blitRegion.srcOffsets[1].x = srcSize.width;
	blitRegion.srcOffsets[1].y = srcSize.height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dstSize.width;
	blitRegion.dstOffsets[1].y = dstSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	blitInfo.dstImage = destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(cmd, &blitInfo);
}

void VulkanPlatform::flush_image(VkCommandBuffer cmd)
{
	VkClearColorValue clearValue = { {0.0f, 0.0f, 0.0f, 1.0f} };

	VkImageSubresourceRange clearRange{};
	clearRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	clearRange.baseMipLevel = 0;
	clearRange.levelCount = VK_REMAINING_MIP_LEVELS;
	clearRange.baseArrayLayer = 0;
	clearRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	vkCmdClearColorImage(cmd, drawImage_.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}

void VulkanPlatform::Draw()
{
	vkWaitForFences(device_, 1, &getCurrentFrame().renderFence_, true, 10000000000);
	getCurrentFrame().deletionQueue_.flush();
	vkResetFences(device_, 1, &getCurrentFrame().renderFence_);

	uint32_t swapchainImageIndex;
	vkAcquireNextImageKHR(device_, swapchain_, 10000000000, getCurrentFrame().swapchainSemaphore_, nullptr, &swapchainImageIndex);

	VkCommandBuffer cmd = getCurrentFrame().mainCommandBuffer_;

	vkResetCommandBuffer(cmd, 0);

	VkCommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = nullptr;
	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	drawExtent_.width = drawImage_.imageExtent.width;
	drawExtent_.height = drawImage_.imageExtent.height;

	//begin recording commands to the GPU
	vkBeginCommandBuffer(cmd, &cmdBeginInfo);

	transition_image(cmd, drawImage_.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	flush_image(cmd);

	transition_image(cmd, drawImage_.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	transition_image(cmd, swapchainImages_[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copy_image_to_image(cmd, drawImage_.image, swapchainImages_[swapchainImageIndex], drawExtent_, swapchainExtent_);

	transition_image(cmd, swapchainImages_[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	
	vkEndCommandBuffer(cmd);

	//prepare to submit command buffer
	VkCommandBufferSubmitInfo cmdInfo = {};
	cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdInfo.pNext = nullptr;
	cmdInfo.commandBuffer = cmd;
	cmdInfo.deviceMask = 0;

	VkSemaphoreSubmitInfo waitInfo;
	waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waitInfo.pNext = nullptr;
	waitInfo.semaphore = getCurrentFrame().swapchainSemaphore_;
	waitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
	waitInfo.deviceIndex = 0;
	waitInfo.value = 1;

	VkSemaphoreSubmitInfo signalInfo;
	signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signalInfo.pNext = nullptr;
	signalInfo.semaphore = getCurrentFrame().renderSemaphore_;
	signalInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	signalInfo.deviceIndex = 0;
	signalInfo.value = 1;

	VkSubmitInfo2 submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submit.pNext = nullptr;

	submit.waitSemaphoreInfoCount = 1;
	submit.pWaitSemaphoreInfos = &waitInfo;

	submit.signalSemaphoreInfoCount = 1;
	submit.pSignalSemaphoreInfos = &signalInfo;

	submit.commandBufferInfoCount = 1;
	submit.pCommandBufferInfos = &cmdInfo;

	vkQueueSubmit2(graphicsQueue_, 1, &submit, getCurrentFrame().renderFence_);

	//prepare to present
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &swapchain_;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &getCurrentFrame().renderSemaphore_;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	vkQueuePresentKHR(graphicsQueue_, &presentInfo);

	frameNumber_++;
}
