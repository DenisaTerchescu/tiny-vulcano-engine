#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class TinyEngine;

	class TinyWindow {
	public:
		TinyWindow();
		~TinyWindow();

		void initWindow();
		void cleanUp();

		GLFWwindow* getWindow() const;
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		const int width = WIDTH;
		const int height = HEIGHT;

		GLFWwindow *window;

  };
