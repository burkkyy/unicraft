/**
 * @file window.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace uni {
namespace eng {

/**
 * @brief Creates a window to render onto
 *
 * Class creates a GLFW window and surface we can render onto.
 * 
 */
class Window {
public:
	Window(int width, int height, const std::string& name);
	~Window();

	// Prevents copying
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

private:
	static void resize_callback(GLFWwindow* window, int width, int height);
	void init();

	int width, height;
	std::string name;
	GLFWwindow* window;
};

}	// namespace eng
}	// namespace uni
