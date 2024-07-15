#pragma once
#include "TinyWindow.hpp"
#include "TinyEngine.hpp"

	TinyWindow:: TinyWindow(){ }

	TinyWindow::~TinyWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();

	}

	void TinyWindow::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, "Tiny tiny window", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, keyCallback);
	}

	void TinyWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<TinyEngine*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
	void TinyWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			// Handle key press event
			switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
				// Handle other keys here
			}
		}
		auto engine = reinterpret_cast<TinyEngine*>(glfwGetWindowUserPointer(window));
		if (engine) {
			engine->onKey(key, action);
		}
	}


	void TinyWindow::cleanUp() {
		glfwDestroyWindow(window);
	}


	GLFWwindow* TinyWindow::getWindow() const {
		return window;
	}
