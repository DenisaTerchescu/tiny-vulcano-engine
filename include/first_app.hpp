#pragma once

#include "vk_window.hpp"
#include "vk_pipeline.hpp"

namespace vk {
	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VKWindow window{ WIDTH, HEIGHT, "Cute lil window" };
		VKPipeline vkPipeline{ "C:/Users/Denisa/Desktop/TinyVulcano/resources/shaders/simple_shader.vert.spv",  "C:/Users/Denisa/Desktop/TinyVulcano/resources/shaders/simple_shader.frag.spv" };

	};
}