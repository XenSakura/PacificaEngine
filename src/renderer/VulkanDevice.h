#pragma once
#include "PacificaCommon.h"
#include <optional>
namespace Vulkan
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional <uint32_t> presentFamily;
        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice pDevice, const VkSurfaceKHR& surface);
	bool pickPhysicalDevice(VkPhysicalDevice& physicalDevice, VkInstance& instance, VkSurfaceKHR& surface);
	bool createLogicalDevice(RenderContext& context, VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface);
	bool destroyLogicalDevice(RenderContext& context);
}
