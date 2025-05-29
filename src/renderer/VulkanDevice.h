#include "vulkan/vulkan.hpp"
#include <vector>
class VulkanDevice
{
public:
	VulkanDevice(vk::Instance& instance, vk::SurfaceKHR& surface);
	~VulkanDevice();
	vk::Device m_device;
	vk::PhysicalDevice m_pdevice;
	vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;
	std::vector<uint32_t> FamilyIndices;
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;
private:
};
