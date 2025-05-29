#pragma once
#include "vulkan/vulkan.hpp"
#include <unordered_map>
#include <string>
struct VulkanPipeline
{
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
	VulkanPipelineBuilder(vk::Device& device, vk::Extent2D extent);
	~VulkanPipelineBuilder();
	VulkanPipeline& BuildPipeline();
	VulkanPipeline& GetPipeline();
private:
	std::unordered_map<std::string, VulkanPipeline> m_pipelineMap;
	vk::Device& m_device;
	vk::Extent2D& m_extent;
};
