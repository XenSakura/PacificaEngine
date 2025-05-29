#include "vulkan/vulkan.hpp"

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();
	vk::Instance m_instance;
	vk::detail::DispatchLoaderDynamic m_dldi;
private:
};
