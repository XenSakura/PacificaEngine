#include "PacificaCommon.h"

namespace Vulkan
{
	bool createSwapchain(RenderContext& context, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);

	bool destroySwapchain(RenderContext& context);

	bool createSwapchainImageViews(RenderContext& context);
}
