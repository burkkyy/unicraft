#include "engine.hpp"

namespace gfx {

Engine::Engine(){
    swapchain = new SwapChain(device, win.get_extent());
}

Engine::~Engine(){
    delete swapchain;
}

void Engine::run(){
    Pipeline p(device, "shaders/vert.spv", "shaders/frag.spv");

    while(!win.should_close()){
        glfwPollEvents();
    }
}

}
