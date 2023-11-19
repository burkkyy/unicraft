/**
 * @file window.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include "window.hpp"

namespace uni {
namespace eng {

/**
 * @brief Constructor
 * @note Only constructor for Window class
 * @param[in] width Width of new window
 * @param[in] height Height of new window
 * @param[in] name Name of new window
 */
Window::Window(int width, int height, const std::string& name)
: width{width}, height{height}, name{name} {
    init();
}

/**
 * @brief Default Deconstructor
 */
Window::~Window(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::resize_callback(GLFWwindow* window, int width, int height){
    (void)window;
    (void)width;
    (void)height;
}

/**
 * @brief Creates window
 * @note Called by constructor
 * @return void
 */
void Window::init(){
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create GLFW window
    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    
    // Set function to call on any window resize event
    glfwSetFramebufferSizeCallback(window, resize_callback);
}

}   // namespace eng
}   // namespace uni
