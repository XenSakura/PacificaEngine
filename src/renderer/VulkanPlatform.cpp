#include "VulkanPlatform.h"
#include <memory>
#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
using namespace Vulkan;

VulkanPlatform::VulkanPlatform()
	:m_window(Window::getInstance()), 
	m_context({}), 
	m_initialized(false), 
	m_debugMessenger(VK_NULL_HANDLE),
	m_instance(VK_NULL_HANDLE),
	m_physicalDevice(VK_NULL_HANDLE),
	m_surface(VK_NULL_HANDLE)
{
}

void VulkanPlatform::Initialize()
{
	m_initialized = true;

	bool instance_result = createVkInstance(m_instance);
	bool debug_result = setupDebugMessenger(m_instance, m_debugMessenger);
	bool surface_result = createVkSurface(m_instance, m_surface);
	bool physicaldevice_result = pickPhysicalDevice(m_physicalDevice, m_instance, m_surface);
	bool device_result = createLogicalDevice(m_context, m_physicalDevice, m_surface);
	bool swapchain_result = createSwapchain(m_context, m_physicalDevice, m_surface);
	bool imageviews_result = createSwapchainImageViews(m_context);

}

void VulkanPlatform::Shutdown()
{
	

	destroySwapchain(m_context);

	destroyLogicalDevice(m_context);

	destroyVkSurface(m_instance, m_surface);

	destroyDebugMessenger(m_instance, m_debugMessenger);

	destroyVkInstance(m_instance);


	m_initialized = false;
}

VulkanPlatform::~VulkanPlatform()
{
	if (m_initialized)
		throw std::runtime_error("VulkanPlatform not shutdown properly!");
}
