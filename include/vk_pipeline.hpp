#pragma once

#include<string>
#include<vector>
#include "vk_device.hpp"
namespace vk {
	class VKPipeline {

		struct PipelineConfigInfo {};

	public:
		VKPipeline(
			VKDevice& device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo);
		~VKPipeline() {};

		VKPipeline(const VKPipeline&) = delete; 
		void operator=(const VKPipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);


	private:
		static std::vector<char> readFile(const std::string& filePath);
		void createGraphicsPipeline(
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char> code,VkShaderModule* shaderModule );
		VkDevice& vkDevice;
		VKPipeline& graphicsPipeline;
		VkShaderModule vertModule;
		VkShaderModule fragModule;

	};
}