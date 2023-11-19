/** 
 * @brief entry for game
 * @file test.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include "logger.hpp"
#include "window.hpp"
#include "device.hpp"

int main(int argc, const char** argv){
	(void)argc;
	(void)argv;


    using namespace uni;
    UPDATE("Test", "Starting test...");
    {
        // Test window
        eng::Window window(1000, 1000, "test");
        for(size_t i = 0; i < 100; i++){
            glfwPollEvents();
        }

        // Test Device
        eng::Device device(window);
    }
    INFO("Test", "Finished test.");
	return 0;
}
