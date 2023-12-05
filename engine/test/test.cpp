/** 
 * @brief entry for game
 * @file test.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include <iostream>

#include "window.hpp"
#include "device.hpp"

#define TEST_START(...)	std::cout << __VA_ARGS__ << "..... "
#define TEST_OK()	std::cout << "\033[1;32m[OK]\033[0m" << '\n'
#define TEST_FAIL()	std::cout << "\033[1;31m[FAIL]\033[0m" << '\n'
#define MSG(...)	std::cout << "\033[1;34m[TEST]\033[0m " << __VA_ARGS__ << '\n'
#define TEST_ASSERT(b)	if(b){ TEST_OK(); } else { TEST_FAIL(); } 

int main(int argc, const char** argv){
	(void)argc;
	(void)argv;

	using namespace uni;
	MSG("Starting testing.");
    
	TEST_START("Testing window");

    // Test window creation and deletion
    auto* window = new eng::Window(1000, 1000, "tets");
    for(size_t i = 0; i < 100; i++){ glfwPollEvents(); }
	delete window;

	TEST_OK();

	TEST_START("Testing device");
	{	        
		eng::Window win(100, 100, "Test");
		eng::Device device(win);
	}
	TEST_OK();

    MSG("Finished testing.");
	return 0;
}

