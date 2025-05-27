#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanImageViews.h"
#include <GLFW/glfw3.h>
#include "..\core\Window.h"
#include <algorithm>
namespace Vulkan
{
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(Window::getInstance().get(), &width, &height);
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }

    }

	bool createSwapchain(RenderContext& context, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
	{
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        //if the queue for presenting images to the screen is separate for the queue for rendering graphics
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(context.m_device, &createInfo, nullptr, &context.m_swapchain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(context.m_device, context.m_swapchain, &imageCount, nullptr);
        context.m_swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(context.m_device, context.m_swapchain, &imageCount, context.m_swapchainImages.data());

        context.m_swapchainImageFormat = surfaceFormat.format;
        context.m_swapchainExtent = extent;
        return true;
	}

	bool destroySwapchain(RenderContext& context)
	{
        vkDeviceWaitIdle(context.m_device);
        //don't forget to do color image view, color image, and color image memory

        //same with depth image view, depth image, and depth image memory

        //same with swapchain image views
        vkDestroySwapchainKHR(context.m_device, context.m_swapchain, nullptr);
        return true;
	}

    bool createSwapchainImageViews(RenderContext& context)
    {
        context.m_swapchainImageViews.resize(context.m_swapchainImages.size());

        for (size_t i = 0; i < context.m_swapchainImages.size(); ++i)
        {
            context.m_swapchainImageViews[i] = createImageView(context.m_swapchainImages[i], context.m_swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }
        return true;
    }

    bool destroySwapchainImageViews(RenderContext& context)
    {
        for (auto imageView : context.m_swapchainImageViews)
        {
            vkDestroyImageView(context.m_device, imageView, nullptr);
        }
        return true;
    }

}
