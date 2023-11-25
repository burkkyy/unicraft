/**
 * @file engine/include/swapchain.hpp
 * @author Caleb Burke
 * @date Nov 20, 2023
 */

#pragma once

namespace uni {
namespace eng {

class SwapChain {
public:
    SwapChain(Device& device);
    ~SwapChain();

    // Prevents copying
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

private:
    void initialize();
    void create_swapchain();

    Device& device;
    VkSwapchainKHR swapchain;
};

}   // namespace eng
}   // namespace uni
