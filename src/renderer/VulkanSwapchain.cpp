#include "VulkanSwapchain.h"

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, vk::SurfaceKHR& surface, uint32_t width, uint32_t height)
    :m_deviceRef(device),
    m_surface(surface),
    m_width(width),
    m_height(height)
{
    createSwapchain();
    createImageViews();
}

VulkanSwapchain::~VulkanSwapchain()
{
    destroySwapchain();
}

void VulkanSwapchain::createSwapchain()
{

    struct SM {
        vk::SharingMode sharingMode;
        uint32_t familyIndicesCount;
        uint32_t* familyIndicesDataPtr;
    } sharingModeUtil{ (m_deviceRef.graphicsQueueFamilyIndex != m_deviceRef.presentQueueFamilyIndex) ?
                           SM{ vk::SharingMode::eConcurrent, 2u, m_deviceRef.FamilyIndices.data() } :
                           SM{ vk::SharingMode::eExclusive, 0u, static_cast<uint32_t*>(nullptr) } };

    auto capabilities = m_deviceRef.m_pdevice.getSurfaceCapabilitiesKHR(m_surface);
    auto formats = m_deviceRef.m_pdevice.getSurfaceFormatsKHR(m_surface);

    m_format = vk::Format::eB8G8R8A8Unorm;
    m_extent = vk::Extent2D{ m_width, m_height };

    vk::SwapchainCreateInfoKHR swapChainCreateInfo({}, m_surface, swapchainImagesCount, m_format,
        vk::ColorSpaceKHR::eSrgbNonlinear, m_extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
        sharingModeUtil.sharingMode, sharingModeUtil.familyIndicesCount,
        sharingModeUtil.familyIndicesDataPtr, vk::SurfaceTransformFlagBitsKHR::eIdentity,
        vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eFifo, true, nullptr);

    m_swapchain = m_deviceRef.m_device.createSwapchainKHR(swapChainCreateInfo);

    m_images = m_deviceRef.m_device.getSwapchainImagesKHR(m_swapchain);

}

void VulkanSwapchain::destroySwapchain()
{
    for (auto& view : m_imageViews)
    {
        m_deviceRef.m_device.destroyImageView(view);
    }
    m_deviceRef.m_device.destroySwapchainKHR(m_swapchain);
}

void VulkanSwapchain::createImageViews()
{
    m_imageViews.clear();
    m_imageViews.reserve(m_images.size());
    for (auto image : m_images) {
        vk::ImageViewCreateInfo imageViewCreateInfo(vk::ImageViewCreateFlags(), image,
            vk::ImageViewType::e2D, m_format,
            vk::ComponentMapping{ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
                vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA },
            vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
        m_imageViews.push_back(m_deviceRef.m_device.createImageView(imageViewCreateInfo));
    }
}

void VulkanSwapchain::recreateSwapchain(uint32_t newWidth, uint32_t newHeight)
{
    m_width = newWidth;
    m_height = newHeight;
    m_deviceRef.m_device.waitIdle(); // Wait for device to finish using old swapchain
    destroySwapchain();  // Destroy image views + old swapchain
    createSwapchain(); // Recreate with new size
    createImageViews(); // Recreate image views
}
