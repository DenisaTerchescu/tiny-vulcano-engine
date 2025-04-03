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
		glfwSetCursorPosCallback(window, cursorPositionCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetKeyCallback(window, keyCallback);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	void TinyWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {

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
			realWindow->input.resetTinyInput();
		}

	}

	void TinyWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
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
		realWindow->input.keyBoard[index + TinyButton::A].state = state;

	}else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		int index = key - GLFW_KEY_0;
		realWindow->input.keyBoard[index + TinyButton::NR0].state = state;
	}else
	{
	//special keys
		//GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT

		if (key == GLFW_KEY_SPACE)
		{
			realWindow->input.keyBoard[TinyButton::Space].state = state;
		}
		else
		if (key == GLFW_KEY_ENTER)
		{
			realWindow->input.keyBoard[TinyButton::Enter].state = state;
		}
		else
		if (key == GLFW_KEY_ESCAPE)
		{
			realWindow->input.keyBoard[TinyButton::Escape].state = state;
		}
		else
		if (key == GLFW_KEY_UP)
		{
			realWindow->input.keyBoard[TinyButton::Up].state = state;
		}
		else
		if (key == GLFW_KEY_DOWN)
		{
			realWindow->input.keyBoard[TinyButton::Down].state = state;
		}
		else
		if (key == GLFW_KEY_LEFT)
		{
			realWindow->input.keyBoard[TinyButton::Left].state = state;
		}
		else
		if (key == GLFW_KEY_RIGHT)
		{
			realWindow->input.keyBoard[TinyButton::Right].state = state;
		}
		else
		if (key == GLFW_KEY_LEFT_CONTROL)
		{
			realWindow->input.keyBoard[TinyButton::LeftCtrl].state = state;
		}else
		if (key == GLFW_KEY_TAB)
		{
			realWindow->input.keyBoard[TinyButton::Tab].state = state;
		}else
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			realWindow->input.keyBoard[TinyButton::LeftShift].state = state;
		}else
		if (key == GLFW_KEY_LEFT_ALT)
		{
			realWindow->input.keyBoard[TinyButton::LeftAlt].state = state;
		}
	}
	}

	void TinyWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		auto realWindow = reinterpret_cast<TinyWindow*>(glfwGetWindowUserPointer(window));

		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			realWindow->input.leftMouse.state = (action == GLFW_PRESS) ? 1 : 0;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			realWindow->input.rightMouse.state = (action == GLFW_PRESS) ? 1 : 0;
		}
	}

	void TinyWindow::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		auto realWindow = reinterpret_cast<TinyWindow*>(glfwGetWindowUserPointer(window));
		realWindow->input.lastPos = realWindow->input.mousePos;
		realWindow->input.mousePos.x = static_cast<int>(xpos);
		realWindow->input.mousePos.y = static_cast<int>(ypos);
		realWindow->input.delta = realWindow->input.mousePos - realWindow->input.lastPos;
	}


	void TinyWindow::cleanUp() {
		glfwDestroyWindow(window);
	}


	GLFWwindow* TinyWindow::getWindow() const {
		return window;
	}
