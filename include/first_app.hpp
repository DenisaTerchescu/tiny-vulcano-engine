#pragma once

#include "vk_window.hpp"
#include "vk_pipeline.hpp"
#include "vk_device.hpp"

namespace vk {
	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VKWindow window{ WIDTH, HEIGHT, "Cute lil window" };
		VKDevice device{ window };
		VKPipeline vkPipeline{ 
			device, 
			"C:/Users/Denisa/Desktop/TinyVulcano/resources/shaders/simple_shader.vert.spv",  
			"C:/Users/Denisa/Desktop/TinyVulcano/resources/shaders/simple_shader.frag.spv", 
			VKPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

	};
}