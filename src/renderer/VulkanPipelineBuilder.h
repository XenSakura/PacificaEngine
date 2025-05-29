#pragma once
#include "vulkan/vulkan.hpp"
#include <unordered_map>
#include <string>
#include "ShaderManager.h"
#include "VulkanSwapchain.h"
struct VulkanPipeline
{
	std::string pipelineName;
	vk::Pipeline m_pipeline;
	vk::PipelineLayout m_layout;
};

struct PipelineDescriptor {
	std::string vertexShader;
	std::string fragmentShader;
	bool depthTest;
	bool blending;
	vk::CullModeFlagBits cullMode;
	std::vector<vk::DescriptorSetLayout> descriptorLayouts;
	std::vector<vk::PushConstantRange> pushConstants;
};



class VulkanPipelineBuilder
{
public:
	VulkanPipelineBuilder(const vk::Device& device, const VulkanSwapchain& swapchain);
	~VulkanPipelineBuilder();
	void BuildPipeline(const std::string& name, VulkanShader& vertShader, VulkanShader& fragShader);
	VulkanPipeline& GetPipeline(const std::string& name);
private:
	std::unordered_map<std::string, VulkanPipeline> m_pipelineMap;
	const vk::Device& m_device;
	const VulkanSwapchain& m_swapchain;
};
