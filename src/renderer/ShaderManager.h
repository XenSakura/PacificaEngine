#pragma once
#include <shaderc/shaderc.hpp>
#include <unordered_map>
#include <filesystem>
#include <string>
#include "vulkan/vulkan.hpp"
struct VulkanShader
{
	std::string name;
	std::string path;
	shaderc_shader_kind kind;
	vk::ShaderModule module = VK_NULL_HANDLE;
	std::filesystem::file_time_type lastModified;
};

class ShaderManager
{
private:
	vk::Device& m_device;
	shaderc::Compiler m_compiler;
	shaderc::CompileOptions m_options;
	std::unordered_map<std::string, VulkanShader> m_shaderMap;
	void CompileShader(VulkanShader& entry);
	std::string readFileToString(const std::string& path);
public:
	ShaderManager(vk::Device& device);
	~ShaderManager();
	//for when we want to add shaders in real time and hot compile them
	void Add(const std::string& name, const std::string& path, shaderc_shader_kind kind);
	void CompileAll(const std::string& directory);
	void HotReload();

	vk::ShaderModule get(const std::string name);
};
