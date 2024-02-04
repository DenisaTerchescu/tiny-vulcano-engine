#pragma once

#include "vk_window.hpp"

namespace vk {
	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VKWindow window{ WIDTH, HEIGHT, "Cute lil window" };

	};
}