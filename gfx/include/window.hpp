#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "usr.hpp"

namespace gfx {

class Window {
public:
    Window(int, int);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool should_close(){ return glfwWindowShouldClose(win); }
    VkExtent2D get_extent(){ return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    void create_surface(VkInstance, VkSurfaceKHR*);

private:
    int width;
    int height;
    const char* NAME = "unicraft";
    GLFWwindow* win;
};

}
