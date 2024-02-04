#include "first_app.hpp"


namespace vk {
	void FirstApp::run() {
		while (!window.shouldClose()) {
			glfwPollEvents();
		}
	}
}