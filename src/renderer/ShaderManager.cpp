#include "ShaderManager.h"
#include <fstream>
#include <iostream>

//add support for raytracing

void ShaderManager::CompileShader(VulkanShader& entry)
{
	std::string file = readFileToString(entry.path);
	shaderc::SpvCompilationResult shaderModule = m_compiler.CompileGlslToSpv(file, entry.kind, entry.name.c_str(), m_options);
	if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << "Shader compile failed for " + entry.name + ": " + shaderModule.GetErrorMessage() <<std::endl;
		throw std::runtime_error("Shader compile failed for " + entry.name + ": " + shaderModule.GetErrorMessage());
	}
	std::vector<uint32_t> shaderCode = std::vector<uint32_t>{ shaderModule.cbegin(), shaderModule.cend() };
	ptrdiff_t shaderSize = std::distance(shaderCode.begin(), shaderCode.end());
	vk::ShaderModuleCreateInfo shaderCreateInfo = { {}, shaderSize * sizeof(uint32_t), shaderCode.data() };
	vk::ShaderModule module = m_device.createShaderModule(shaderCreateInfo);
	if (entry.module != VK_NULL_HANDLE)
	{
		m_device.destroyShaderModule(entry.module);
	}
	entry.module = module;
	entry.lastModified = std::filesystem::last_write_time(entry.path);
	std::cout << "[ShaderManager] Compiled shader: " << entry.name << std::endl;
}

std::string ShaderManager::readFileToString(const std::string& path)
{
	
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "[ShaderManager] Failed to open file!" << std::endl;
		return {};
	}
	std::ostringstream contents;
	contents << file.rdbuf();
	return contents.str();

}

ShaderManager::ShaderManager(vk::Device& device)
	:m_device(device)
{
	m_shaderMap.reserve(20);
	m_options.SetOptimizationLevel(shaderc_optimization_level_performance);
}

ShaderManager::~ShaderManager()
{
	for (auto& [name, shader] : m_shaderMap) {
		if (shader.module != VK_NULL_HANDLE) {
			m_device.destroyShaderModule(shader.module);
		}
	}
}

void ShaderManager::Add(const std::string& name, const std::string& path, shaderc_shader_kind kind)
{
	VulkanShader new_shader = VulkanShader(name, path, kind);
	CompileShader(new_shader);
	m_shaderMap[name] = new_shader;
	
}

void ShaderManager::CompileAll(const std::string& directory)
{
	for (const auto& file : std::filesystem::directory_iterator(directory))
	{
		auto ext = file.path().extension();
		shaderc_shader_kind kind;
		if (ext == ".frag") kind = shaderc_fragment_shader;
		else if (ext == ".vert") kind = shaderc_vertex_shader;
		else if (ext == ".comp") kind = shaderc_compute_shader;
		else
		{
			std::cerr << "[ShaderManager] Wrong file in directory! File: " << file.path() << std::endl;;
			continue;
		}
		Add(file.path().filename().string(), file.path().string(), kind);
	}
}

void ShaderManager::HotReload()
{
	for (auto& [name, shader] : m_shaderMap)
	{
		auto newTime = std::filesystem::last_write_time(shader.path);
		if (newTime != shader.lastModified)
		{
			CompileShader(shader);
			shader.lastModified = newTime;
		}
			
	}
}
