#include "VulkanDevice.h"
#include <iostream>
#include <vector>
#include <set>
VulkanDevice::VulkanDevice(vk::Instance& instance, vk::SurfaceKHR& surface)
{
	auto physicalDevices = instance.enumeratePhysicalDevices();

	for (auto& d : physicalDevices) {
		std::cout << d.getProperties().deviceName << "\n";
	}

	m_pdevice = physicalDevices[std::distance(physicalDevices.begin(),
		std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const vk::PhysicalDevice& physicalDevice) {
			return strstr(physicalDevice.getProperties().deviceName, "M1");
			}))];

	auto queueFamilyProperties = m_pdevice.getQueueFamilyProperties();

    graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(),
        std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
            [](vk::QueueFamilyProperties const& qfp) {
                return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
            }));

    for (auto i = 0ul; i < queueFamilyProperties.size(); i++) {
        if (m_pdevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface)) {
            presentQueueFamilyIndex = i;
        }
    }

    std::set<uint32_t> uniqueQueueFamilyIndices = { static_cast<uint32_t>(graphicsQueueFamilyIndex),
        static_cast<uint32_t>(presentQueueFamilyIndex) };

    FamilyIndices = { uniqueQueueFamilyIndices.begin(),
        uniqueQueueFamilyIndices.end() };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 0.0f;
    for (auto& queueFamilyIndex : uniqueQueueFamilyIndices) {
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{ vk::DeviceQueueCreateFlags(),
            static_cast<uint32_t>(queueFamilyIndex), 1, &queuePriority });
    }

    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };

    m_device = m_pdevice.createDevice(vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
        0u, nullptr, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data()));

    m_graphicsQueue = m_device.getQueue(graphicsQueueFamilyIndex, 0);
    m_presentQueue = m_device.getQueue(presentQueueFamilyIndex, 0);


}

VulkanDevice::~VulkanDevice()
{
    m_device.destroy();
}
