#pragma once
#include "PacificaCommon.h"
#include "IAPIPlatform.h"
#include "Window.h"
#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanSurface.h"


//this is the Vulkan backend
class VulkanPlatform : public IAPIPlatform
{
public:
	VulkanPlatform();
	~VulkanPlatform();
private:
	//only need one of each
	Window m_window;
	VulkanInstance m_instance;
	VulkanDebug m_debugMessenger;
	VulkanSurface m_surface;

};