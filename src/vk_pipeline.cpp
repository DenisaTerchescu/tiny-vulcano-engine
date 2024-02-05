#include "vk_pipeline.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace vk{

	VKPipeline::VKPipeline(
		VKDevice& device,
		const std::string& vertFilePath,
		const std::string& fragFilePath,
		const PipelineConfigInfo& configInfo) : vkDevice{device}
	{
		createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
	}

	std::vector<char> VKPipeline::readFile(const std::string& filePath) {
		
		std::ifstream file{ filePath, std::ios::ate | std::ios::binary };

		if(!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filePath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());

		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	 }

	void VKPipeline::createGraphicsPipeline(const std::string& vertFilePath,
		const std::string& fragFilePath, const PipelineConfigInfo& configInfo) {
		auto vertCode = readFile(vertFilePath);
		auto fragCode = readFile(fragFilePath);

		std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
		std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';

	}

	void VKPipeline::createShaderModule(const std::vector<char> code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.size = code.size();

	}
}