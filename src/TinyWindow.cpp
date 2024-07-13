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
	}

	void TinyWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<TinyEngine*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void TinyWindow::cleanUp() {
		glfwDestroyWindow(window);
	}


	GLFWwindow* TinyWindow::getWindow() const {
		return window;
	}
