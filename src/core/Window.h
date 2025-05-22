#pragma once
#include <GLFW/glfw3.h>


const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;

class Window
{
public:
    // Get the singleton instance
    static Window& getInstance() {
        static Window instance;
        return instance;
    }

    // Delete copy/move constructors and assignment operators
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    GLFWwindow* get() const { return window; }
    
    bool framebufferResized = false;

    uint32_t width = WINDOW_WIDTH;
    uint32_t height = WINDOW_HEIGHT;

private:
    

    GLFWwindow* window = nullptr;

    // Private constructor & destructor
    Window();
    ~Window();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};