/**
 * @file src/engine/window.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include "engine/window.hpp"
#include "util/logger.hpp"

namespace uni {
namespace eng {

Window::Window(int width, int height, const std::string& name) : width{width}, height{height}, name{name} {
	initialize();
}

Window::~Window(){
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::create_surface(VkInstance instance, VkSurfaceKHR* surface){
	if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
		VK_ERROR("Failed to create VkSurfaceKHR.");
	}
	VK_INFO("Created VkSurfaceKHR.");
}

void Window::resize_callback(GLFWwindow* window, int width, int height){
	(void)window;
	(void)width;
	(void)height;
}
	
void Window::initialize(){
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

}	// namespace eng
}	// namespace uni

