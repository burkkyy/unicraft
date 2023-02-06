#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"

namespace gfx {

class Engine {
public:
    const int WIDTH = 800;
    const int HEIGHT = 600;
    const char* NAME = "unicraft";

    Engine();
    ~Engine();

    void run();

private:
    Window win{WIDTH, HEIGHT};
    Device device{win};
    SwapChain* swapchain;
};

}
