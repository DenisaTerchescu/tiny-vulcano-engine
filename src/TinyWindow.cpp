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
		std::cout << "First pointer: " << (void*)this << '\n';
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, keyCallback);
	}

	void TinyWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		//auto app = reinterpret_cast<TinyEngine*>(glfwGetWindowUserPointer(window));
		//app->framebufferResized = true;

		auto realWindow = reinterpret_cast<TinyWindow*>(glfwGetWindowUserPointer(window));
		realWindow->framebufferResized = true;
	}

	static void focusCallback(GLFWwindow* window, int focused)
	{
		auto realWindow = reinterpret_cast<TinyWindow*>(glfwGetWindowUserPointer(window));

		if (focused)
		{
			realWindow->focused = 1;
		}
		else
		{
			realWindow->focused = 0;
			//if you not capture the release event when the window loses focus,
			//the buttons will stay pressed
			realWindow->input.resetInput();
		}

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

	auto realWindow = reinterpret_cast<TinyWindow*>(glfwGetWindowUserPointer(window));
		
	bool state = 0;
	if (action == GLFW_PRESS)
	{
		state = 1;
	}
	else if (action == GLFW_RELEASE)
	{
		state = 0;
	}
	else
	{
		return;
	}

	if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
	{
		int index = key - GLFW_KEY_A;
		realWindow->input.keyBoard[index + Button::A].state = state;

	}else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		int index = key - GLFW_KEY_0;
		realWindow->input.keyBoard[index + Button::NR0].state = state;
	}else
	{
	//special keys
		//GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT

		if (key == GLFW_KEY_SPACE)
		{
			realWindow->input.keyBoard[Button::Space].state = state;
		}
		else
		if (key == GLFW_KEY_ENTER)
		{
			realWindow->input.keyBoard[Button::Enter].state = state;
		}
		else
		if (key == GLFW_KEY_ESCAPE)
		{
			realWindow->input.keyBoard[Button::Escape].state = state;
		}
		else
		if (key == GLFW_KEY_UP)
		{
			realWindow->input.keyBoard[Button::Up].state = state;
		}
		else
		if (key == GLFW_KEY_DOWN)
		{
			realWindow->input.keyBoard[Button::Down].state = state;
		}
		else
		if (key == GLFW_KEY_LEFT)
		{
			realWindow->input.keyBoard[Button::Left].state = state;
		}
		else
		if (key == GLFW_KEY_RIGHT)
		{
			realWindow->input.keyBoard[Button::Right].state = state;
		}
		else
		if (key == GLFW_KEY_LEFT_CONTROL)
		{
			realWindow->input.keyBoard[Button::LeftCtrl].state = state;
		}else
		if (key == GLFW_KEY_TAB)
		{
			realWindow->input.keyBoard[Button::Tab].state = state;
		}else
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			realWindow->input.keyBoard[Button::LeftShift].state = state;
		}else
		if (key == GLFW_KEY_LEFT_ALT)
		{
			realWindow->input.keyBoard[Button::LeftAlt].state = state;
		}
	}

		
		//auto engine = reinterpret_cast<TinyEngine*>(glfwGetWindowUserPointer(window));
		//std::cout << (void*)glfwGetWindowUserPointer(window) << "\n";
		//
		//if (engine) {
		//	engine->onKey(key, action);
		//}
	}


	void TinyWindow::cleanUp() {
		glfwDestroyWindow(window);
	}


	GLFWwindow* TinyWindow::getWindow() const {
		return window;
	}
