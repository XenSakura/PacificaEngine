#include "vulkan/vulkan.hpp"
#include <vector>
#include "VulkanDevice.h"

class VulkanSwapchain
{
public:
	VulkanSwapchain(VulkanDevice& device, vk::SurfaceKHR& surface, uint32_t width, uint32_t height);
	~VulkanSwapchain();
	vk::SwapchainKHR m_swapchain;
	vk::Extent2D m_extent;
	vk::Format m_format;
	std::vector<vk::Image> m_images;
	std::vector<vk::ImageView> m_imageViews;
	uint32_t swapchainImagesCount = 2;
	uint32_t m_width, m_height;
private:
	void createSwapchain();
	void destroySwapchain();
	void createImageViews();
	VulkanDevice& m_deviceRef;
	vk::SurfaceKHR& m_surface;
	void recreateSwapchain(uint32_t newWidth, uint32_t newHeight)
};
