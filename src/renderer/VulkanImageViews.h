#pragma once
#include "PacificaCommon.h"

namespace Vulkan
{
	VkImageView createImageView(VkImage& image, VkFormat& imageFormat, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
}
