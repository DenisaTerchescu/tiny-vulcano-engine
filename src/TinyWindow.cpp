#pragma once
#include "TinyWindow.hpp"


	TinyWindow:: TinyWindow(){ }

	TinyWindow::~TinyWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();

	}
	
	//void TinyWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	//	auto app = reinterpret_cast<TinyVulcanoEngine*>(glfwGetWindowUserPointer(window));
	//	app->framebufferResized = true;
	//}

	void TinyWindow::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, "Tiny tiny window", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		//glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}


	GLFWwindow* TinyWindow::getWindow() const {
		return window;
	}
