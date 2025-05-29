#include "vulkan/vulkan.hpp"

class VulkanDebug
{
public:
	VulkanDebug(vk::Instance& instance, vk::detail::DispatchLoaderDynamic& dldi);
	~VulkanDebug();
	vk::DebugUtilsMessengerEXT m_debugMessenger;
private:
	//only for reference in this class
	vk::Instance& m_instance;
	vk::detail::DispatchLoaderDynamic& m_dldi;
};
