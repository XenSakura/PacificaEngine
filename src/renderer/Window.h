#pragma once
#include <GLFW/glfw3.h>


const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;

class Window
{
public:

    GLFWwindow* window = nullptr;
    
    Window();
    ~Window();

    uint32_t width = WINDOW_WIDTH;
    uint32_t height = WINDOW_HEIGHT;

    bool framebufferResized = false;

private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};