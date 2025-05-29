#include "VulkanInstance.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <iostream>


VulkanInstance::VulkanInstance()
{
	vk::ApplicationInfo appInfo("PacificaEngine", VK_MAKE_VERSION(1, 0, 0),
								"PacificaEngine", VK_MAKE_VERSION(1, 0, 0), 
								VK_API_VERSION_1_3);
	unsigned glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> glfwExtensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
    glfwExtensionsVector.push_back("VK_EXT_debug_utils");
    glfwExtensionsVector.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };

    m_instance = vk::createInstance (vk::InstanceCreateInfo{
        vk::InstanceCreateFlags{ VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR }, &appInfo,
        static_cast<uint32_t>(layers.size()), layers.data(),
        static_cast<uint32_t>(glfwExtensionsVector.size()), glfwExtensionsVector.data() });

    m_dldi = vk::detail::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);
}

VulkanInstance::~VulkanInstance()
{
    m_instance.destroy();
}
