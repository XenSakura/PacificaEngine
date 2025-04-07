#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
	Window(uint32_t width, uint32_t height, const std::string& title);
	~Window();

private:
	GLFWwindow* window;
	std::string windowName;
	uint32_t width, height;
};
