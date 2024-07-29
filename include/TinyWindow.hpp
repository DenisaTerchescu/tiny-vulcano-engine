#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <input.hpp>

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
		static void focusCallback(GLFWwindow* window, int focused);
		void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

		bool focused = 1;

		const int width = WIDTH;
		const int height = HEIGHT;

		bool framebufferResized = 0;

		GLFWwindow *window;

		Input input;

  };
