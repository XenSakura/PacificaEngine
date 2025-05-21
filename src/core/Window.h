#pragma once
#include <GLFW/glfw3.h>
#include <string>


class Window
{
private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;


	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

public:
	Window();
	~Window();
	bool framebufferResized = false;
	GLFWwindow* window = nullptr;
};
