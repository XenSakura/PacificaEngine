#pragma once
#include "PacificaCommon.h"
#include "IAPIPlatform.h"
#include "..\core\Window.h"



//this is the Vulkan backend
class VulkanPlatform : public IAPIPlatform
{
public:
	VulkanPlatform();
	~VulkanPlatform();
	bool m_initialized;
	void Initialize() override;

	void Shutdown() override;
private:
	//only need one of each

	RenderContext m_context;

	Window& m_window;

	VkDebugUtilsMessengerEXT m_debugMessenger;
	
	VkInstance m_instance;

	VkSurfaceKHR m_surface;

	VkPhysicalDevice m_physicalDevice;



};